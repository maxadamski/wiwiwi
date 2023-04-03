import toml
import json
import lzma
import numpy as np
import multiprocessing as mp
import random
from sklearn.naive_bayes import GaussianNB
from fastapi import FastAPI, Depends, Body, Header, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from src.utils import *
from geopy.distance import distance as geodistance
from pprint import pprint

config = toml.load('config.toml')

BAD_REQUEST = HTTPException(400, 'Malformed request')
BAD_LOGIN = HTTPException(400, 'Incorrect username or password')
BAD_TOKEN = HTTPException(401, 'Invalid credentials', {'WWW-Authenticate': 'Bearer'})
SESSION_EXPIRED = HTTPException(403, 'Session expired')
NOT_FOUND = HTTPException(404, 'Not found')

LOGIN_TIMEOUT_HOURS = config['security']['login_timeout_hours']
DEBUG_ENDPOINTS = config['debug_endpoints']
EXPLORATION_RATE = config['exploration_rate']
DOG_VECTOR_SIZE = 256

def filtered_dogs_for_user(user):
    user_coord = user['location']['lat'], user['location']['lon']
    max_dist = user['filter']['max_distance']

    def dog_matches(dog):
        by_dist = max_dist is None or geodistance(user_coord, dog['location']['coord']) < max_dist
        by_new = dog['id'] not in user['liked_dogs'] and dog['id'] not in user['disliked_dogs']
        return by_dist

    return [id for id, x in dogs.items() if dog_matches(x)]

def refill_dogs(user, min_count=0, max_count=10, sample=100, exploration=EXPLORATION_RATE):
    assert max_count < sample
    filtered, queued = user['filtered_dogs'], user['queued_dogs']
    if len(queued) > min_count: return # enough dogs in the queue for now
    k = min(len(filtered), max_count - len(queued))

    random.shuffle(filtered)
    sample_dogs = [dogs[id]['vector'] for id in filtered[:sample]]
    sample_pred = user['model'].predict(sample_dogs)

    n_likely = int(k*(1-exploration))
    likely_dogs = [i for i, pred in enumerate(sample_pred) if pred == 1]
    likely_dogs = fastpop_all(filtered, likely_dogs[:n_likely])
    selected = [dict(id=x, mode='likely') for x in likely_dogs]
    #print('add', len(likely_dogs), 'likely')

    n_random = k - len(likely_dogs)
    if n_random > 0:
        filtered, random_dogs = partition(filtered, -n_random)
        selected += [dict(id=x, mode='random') for x in random_dogs]
        #print('add', len(random_dogs), 'random')

    random.shuffle(selected)
    queued += selected
    user['filtered_dogs'] = filtered


# TODO: replace with actual database
with lzma.open('db/dogs.jsonl.xz', 'rt') as f:
    dogs = [json.loads(x) for x in f.readlines()]
    dogs = {x['id']: x for x in dogs}
    for dog in dogs.values():
        dog['location']['coord'] = (dog['location']['lat'], dog['location']['lon'])
        dog['vector'] = np.array(dog['vector'])

# TODO: replace with actual database
with open('db/users.json', 'r') as f:
    users = json.load(f)
    users = {x['login']: x for x in users}
    for user in users.values():
        user['liked_dogs'] = set()
        user['disliked_dogs'] = set()
        user['filtered_dogs'] = filtered_dogs_for_user(user)
        random.shuffle(user['filtered_dogs'])
        user['filtered_dogs'], user['queued_dogs'] = partition(user['filtered_dogs'], -10)
        user['queued_dogs'] = [dict(id=x, mode='random') for x in user['queued_dogs']]
        user['model'] = GaussianNB(priors=[0.5, 0.5])

api = FastAPI()

api.add_middleware(CORSMiddleware,
    allow_origins=config['security']['cors_allow_origins'],
    allow_methods=['*'], allow_headers=['*'])

manager = mp.Manager()

# thread-safe user session dict
# replace with Redis if needed
user_sessions = manager.dict()

if DEBUG_ENDPOINTS:
    async def user_session():
        return dict(usr=users['tester'])

else:
    async def user_session(x_session_username: str = Header(...), x_session_token: str = Header(...)):
        username, token = x_session_username, x_session_token
        sess = user_sessions.get(username)
        if sess is None or not verify_token(token, sess['key']):
            raise BAD_TOKEN
        if utcnow().timestamp() > sess['exp']:
            del sess[username]
            raise SESSION_EXPIRED
        sess['exp'] = utcfuture(hours=LOGIN_TIMEOUT_HOURS).timestamp() # extend timeout
        sess['usr'] = users[username]
        return sess

@api.post('/login')
async def login(username: str = Body(...), password: str = Body(...)):
    user = users.get(username)
    if user is None or not verify_password(password, user['password']):
        raise BAD_LOGIN
    key = make_session_token()
    exp = utcfuture(hours=LOGIN_TIMEOUT_HOURS).timestamp()
    user_sessions[username] = dict(key=hashed_token(key), exp=exp, username=username)
    return dict(session_token=key)

def get_dog(id, contact=False):
    if id not in dogs: return None
    dog = dogs.get(id)
    res = dict(id=id, title=dog['title'], description=dog['description'],
        photos=dog['photos'], location=dog['location'],
        details=dict(age=None, sex=None, breed=None, chip=None, vaccine=None))
    if contact:
        res['url'] = dog['url']
        res['contact'] = dog['contact']
        res['adopted'] = dog['adopted']
    return res

@api.get('/dog/by_id/{id}')
async def get_dog_by_id(id: int):
    dog = get_dog(id)
    if dog is None: raise NOT_FOUND 
    return dog

@api.post('/dog/next')
async def get_next_dog(sess=Depends(user_session)):
    if not sess['usr']['queued_dogs']:
        return None
    next_dog = sess['usr']['queued_dogs'][0]
    dog = get_dog(next_dog['id'])
    dog['mode'] = next_dog['mode']
    return dog

@api.post('/dog/swipe')
async def post_swipe(like: bool, sess=Depends(user_session)):
    if not sess['usr']['queued_dogs']:
        return None
    dog_id = sess['usr']['queued_dogs'].pop(0)['id']
    if like:
        sess['usr']['liked_dogs'].add(dog_id)
    else:
        sess['usr']['disliked_dogs'].add(dog_id)
    sess['usr']['model'].partial_fit([dogs[dog_id]['vector']], [like], [0, 1])
    refill_dogs(sess['usr'])

@api.get('/me/dogs')
async def get_my_dogs(sess=Depends(user_session)):
    return [get_dog(id, contact=True) for id in sess['usr']['liked_dogs']]

if DEBUG_ENDPOINTS:
    @api.get('/me')
    async def get_me(sess=Depends(user_session)):
        usr = sess['usr'].copy()
        usr['liked_dogs'] = list(usr['liked_dogs'])
        usr['disliked_dogs'] = list(usr['disliked_dogs'])
        usr.pop('model')
        return usr
