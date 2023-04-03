import toml
import re
import os, shutil
import multiprocessing as mp
import asyncio as aio

from fastapi import FastAPI, Body, Header, File, UploadFile, Depends, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.httpsredirect import HTTPSRedirectMiddleware
from redis import Redis
from passlib.hash import sha256_crypt
from shutil import unpack_archive
from tempfile import NamedTemporaryFile
from secrets import token_hex
from asyncio.subprocess import PIPE, STDOUT, DEVNULL
aio_shell = aio.create_subprocess_shell
aio_exec = aio.create_subprocess_exec

MAX_BUILD_TIME = 1*60
MAX_RUN_TIME = 3*60

def validate_status(code):
    if code != 0:
        raise HTTPException(500, dict(error='Build failed', code=code))

def pathext(path):
    return os.path.splitext(path)[1]

def isarchive(ext):
    return ext in ['.zip', '.tar', '.gztar', '.bztar', '.xztar']

def unpack_data(data, dst_dir, ext):
    with NamedTemporaryFile(suffix=ext, delete=True) as f:
        f.write(data)
        f.seek(0)
        unpack_archive(f.name, dst_dir)
    files = os.listdir(dst_dir)
    if len(files) == 1:
        inner_dir = os.path.join(dst_dir, files[0])
        if not os.path.isdir(inner_dir): return
        for file in os.listdir(inner_dir):
            inner_path = os.path.join(inner_dir, file)
            outer_path = os.path.join(dst_dir, file)
            os.rename(inner_path, outer_path)
        shutil.rmtree(inner_dir)

async def lxc_shell(name, cmd, **kwargs):
    return await aio_exec('lxc-execute', '-n', name, '--', 'sh', '-c', cmd, **kwargs)

def lxc_gen_name(name):
    return f'{name}-{token_hex(16)}'

async def lxc_clone(name, new_name):
    player_dir = os.path.join(container_home, name, 'root')
    home = os.path.join(container_home, new_name)
    proc = await aio_shell(f'lxc-create -t player -n {new_name} -- --rootfs={home} --sharedir={container_share} --copydir={player_dir}', stdout=DEVNULL, stderr=STDOUT)
    out, _ = await proc.communicate()
    
    if proc.returncode != 0:
        raise HTTPException(500, dict(error='Container clone failed', code=proc.returncode, log=out))

async def lxc_destroy(name):
    proc = await aio_exec('lxc-destroy', '-f', '-n', name)
    await proc.wait()

# init before fork

print('INFO reading configuration file')
config = toml.load('config.toml')
container_home = 'containers'
container_share = 'share'

print('INFO creating shared memory')
manager = mp.Manager()
job_queue = mp.Queue()
job_status = manager.dict()

app = FastAPI()

if config['force_https']:
    app.add_middleware(HTTPSRedirectMiddleware)

app.add_middleware(CORSMiddleware,
                   allow_origins=config['allow_origins'],
                   allow_methods=['*'],
                   allow_headers=['*'])

async def api_token(api_token: str):
    if not sha256_crypt.verify(api_token, config['api_token']):
        raise HTTPException(400, 'Invalid token')

@app.get('/job/{id}')
async def get_job(id: int):
    if id not in job_status:
        raise HTTPException(404)
    return job_status[id]

@app.put('/job/{id}')
async def new_job(id: int, game_id: int, p1_id: int, p2_id: int, is_ref: bool = False):
    p1_original, p2_original = f'player-{p1_id}', f'ref-{p2_id}' if is_ref else f'player-{p2_id}'
    p1_name, p2_name = lxc_gen_name(p1_original), lxc_gen_name(p2_original)
    try:
        await lxc_clone(p1_original, p1_name)
        await lxc_clone(p2_original, p2_name)
        fifos = ' '.join(f'containers/{x}/root/fifo_{y}' for x in [p1_name,p2_name] for y in ['in','out'])
        p1_out = NamedTemporaryFile(delete=True)
        p2_out = NamedTemporaryFile(delete=True)
        p1 = await lxc_shell(p1_name, 'export PATH && cd root && chmod +x run && ./run fifo_in fifo_out', stdout=p1_out, stderr=STDOUT)
        p2 = await lxc_shell(p2_name, 'export PATH && cd root && chmod +x run && ./run fifo_in fifo_out', stdout=p2_out, stderr=STDOUT)
        judge = await aio_shell(f'files/games/{game_id}/judge/run {fifos} 6 30', stdout=PIPE, stderr=PIPE)
        try:
            out, err = await aio.wait_for(judge.communicate(), MAX_RUN_TIME)
            out = out.decode('utf-8').rstrip()
            result = out.split('\n')[-1]
        except aio.TimeoutError:
            result = 'TIMEOUT'
            out, err = '', ''
        job_status[id] = dict(status='in progress', result='UNKNOWN', log=dict(judge='', p1='', p2=''))

        if p1.returncode is None: p1.kill()
        if p2.returncode is None: p2.kill()
        p1_out.seek(0)
        p2_out.seek(0)

        response = dict(status='done', result=result, log=dict(judge=err, p1=p1_out.read(), p2=p2_out.read()))
        p1_out.close()
        p2_out.close()
    finally:
        await lxc_destroy(p1_name)
        await lxc_destroy(p2_name)

    job_status[id] = response
    if not re.match(r'DRAW|(ILLEGAL|TIMEOUT|WINNER) [12]', result):
        raise HTTPException(500, dict(error='Invalid judge output', result=result))
    return response

@app.put('/player/{id}')
async def new_player(id: int, env_id: int = Body(...), data: UploadFile = File(...), automake: bool = Body(True)):
    return await build_player(id, env_id, data, automake)

# TODO: refactor into put /game/{game_id}/ref/{id}
@app.put('/ref_player/{id}')
async def new_ref_player(id: int, game_id: int = Body(...), env_id: int = Body(...), data: UploadFile = File(...)):
    return await build_player(id, env_id, data, automake=False, ref=game_id)

# TODO: refactor into put /game/{game_id}/judge
@app.put('/game/{id}')
async def new_game(id: int, env_id: int = Body(...), data: UploadFile = File(...)):
    game_dir = os.path.join('files', 'games', str(id), 'judge')
    if os.path.exists(game_dir): shutil.rmtree(game_dir)
    os.makedirs(game_dir)
    content = await data.read()
    # TODO: if ext is not archive
    unpack_data(content, game_dir, pathext(data.filename))
    proc = await aio_shell(f'chmod +x build && ./build', cwd=game_dir, stdout=PIPE, stderr=STDOUT)
    out, _ = await proc.communicate()
    validate_status(proc.returncode)
    return dict(log=out)

async def build_player(id, env_id, data, automake, ref=None):
    base_dir = 'players' if ref is None else os.path.join('games', str(ref), 'refs')
    player_dir = os.path.join('files', base_dir, str(id))
    container_name = f'ref-{id}' if ref else f'player-{id}'
    home = os.path.join(container_home, container_name)
    if os.path.exists(player_dir): shutil.rmtree(player_dir)
    os.makedirs(player_dir)

    content = await data.read()
    name = os.path.basename(data.filename)
    ext = pathext(name)
    if isarchive(ext):
        unpack_data(content, player_dir, ext)
    else:
        with open(os.path.join(player_dir, name), 'wb') as f:
            f.write(content)

    proc = await aio_shell(f'lxc-create -t player -n {container_name} -- --rootfs={home} --sharedir={container_share} --copydir={player_dir}', stdout=DEVNULL, stderr=STDOUT)
    out, _ = await proc.communicate()
    validate_status(proc.returncode)

    # TODO: handle automake

    if os.path.exists(os.path.join('containers', container_name, 'root', 'build')):
        proc = await lxc_shell(container_name, 'export PATH && cd root && chmod +x build && ./build', stdout=PIPE, stderr=STDOUT)
        out, _ = await aio.wait_for(proc.communicate(), MAX_BUILD_TIME)
        validate_status(proc.returncode)
    return dict(log=out)

