import secrets
import toml
from passlib.hash import argon2, sha256_crypt
from fastapi import HTTPException
from utils.time import *

config = toml.load('config.toml')
LOGIN_TIMEOUT = config['security']['login_timeout']
SUBMISSION_TIME_LIMIT = config['security']['submission_time_limit']

BAD_REQUEST = HTTPException(400, 'Malformed request')
BAD_LOGIN = HTTPException(403, 'Incorrect username or password')
BAD_TOKEN = HTTPException(401, 'Invalid credentials', {'WWW-Authenticate': 'Bearer'})
FORBIDDEN = HTTPException(403, 'Forbidden')
SESSION_EXPIRED = HTTPException(403, 'Session expired')
NOT_FOUND = HTTPException(404, 'Not found')
CONFLICT = HTTPException(409, 'Conflict')


def hashed_password(password: str) -> bytes:
    return argon2.using(rounds=4, salt_size=128).hash(password)


def verify_password(password: str, hashed: str) -> bool:
    return argon2.verify(password, hashed)


def make_session_token() -> str:
    return secrets.token_hex(32)


def hashed_token(token: str) -> bytes:
    return sha256_crypt.using(rounds=1000).hash(token)


def verify_token(token: str, hash: bytes) -> bytes:
    return sha256_crypt.verify(token, hash)


def create_session(redis, login, key, exp, user_id, next_submit=None):
    if next_submit:
        redis.hset(login, mapping=dict(key=key, exp=exp, user_id=user_id, next_submit=next_submit))
    else:
        redis.hset(login, mapping=dict(key=key, exp=exp, user_id=user_id))


def check_submission_time(redis, login):
    session = redis.hgetall(login)
    next_submit = float(session[b'next_submit'])
    if utcnow().timestamp() < next_submit:
        return False
    else:
        return True


def update_submission_time(redis, login):
    next_submit = utcfuture(minutes=SUBMISSION_TIME_LIMIT).timestamp()
    redis.hset(login, mapping=dict(next_submit=next_submit))
    return next_submit


def validate_session(redis, login: str, token: str):
    session = redis.hgetall(login)
    if not session:
        raise FORBIDDEN
    key = session[b'key']
    if not verify_token(token, key):
        raise BAD_TOKEN
    exp = float(session[b'exp'])
    if utcnow().timestamp() > exp:
        redis.delete(login)
        raise SESSION_EXPIRED
    uid = int(session[b'user_id'])
    return dict(key=key, exp=exp, user_id=uid, login=login)


def delete_session(redis, login: str):
    redis.delete(login)
