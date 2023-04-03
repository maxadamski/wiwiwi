from fastapi import FastAPI, Depends, Body, Header, UploadFile, File
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import HTMLResponse

import json
import toml
import pugsql
from redis import Redis

from utils.security import *
from utils.time import *
from utils.files import *
from models import *

from datetime import date

import httpx
import asyncio

app = FastAPI()

app.add_middleware(CORSMiddleware,
                   allow_origins=['http://localhost:1234'],
                   allow_methods=['*'],
                   allow_headers=['*'],
                   )

# load config file
config = toml.load('config.toml')
db_user = config['db']['user']
db_password = config['db']['password']
db_port = config['db']['port']
db_host = config['db']['host']
db_name = config['db']['name']
servers = config['servers']
supervisor_api = servers['alpha']['url']

# compile database queries from directory
db = pugsql.module('database/queries/')

# initialize database connection
db.connect(f'postgresql+psycopg2://{db_user}:{db_password}@{db_host}:{db_port}/{db_name}', pool_pre_ping=True)

student_sessions = Redis(host='localhost', port=6666, db=0)
teacher_sessions = Redis(host='localhost', port=6667, db=0)


#
# Endpoint dependencies
#

async def student_session(x_session_login: str = Header(...), x_session_token: str = Header(...)):
    login, token = x_session_login, x_session_token
    session = validate_session(student_sessions, login, token)
    return session


async def teacher_session(x_session_login: str = Header(...), x_session_token: str = Header(...)):
    login, token = x_session_login, x_session_token
    session = validate_session(teacher_sessions, login, token)
    return session


#
# App events
#

@app.on_event('startup')
async def startup():
    pass


@app.on_event('shutdown')
async def shutdown():
    pass


#
# Authentication endpoints
#

@app.post('/login')
async def login(user: UserLogin):
    login, password = user.login, user.password
    student = db.get_student_by_login(login=login)
    if student is not None and verify_password(password, student['password']):
        key = make_session_token()
        exp = utcfuture(hours=LOGIN_TIMEOUT).timestamp()
        create_session(student_sessions, login=student['login'], key=hashed_token(key), exp=exp, user_id=student['id'],
                       next_submit=utcnow().timestamp())
        return dict(key=key, exp=exp, is_teacher=False)

    teacher = db.get_teacher_by_login(login=login)
    if teacher is not None and verify_password(password, teacher['password']):
        key = make_session_token()
        exp = utcfuture(hours=LOGIN_TIMEOUT).timestamp()
        create_session(teacher_sessions, login=teacher['login'], key=hashed_token(key), exp=exp,
                       user_id=teacher['id'])
        return dict(key=key, exp=exp, is_teacher=True)

    raise BAD_LOGIN


#
# Student endpoints
#


@app.post('/students')
async def create_student(student: StudentPost, session=Depends(teacher_session)):
    login, password, nickname, group_id = student.login, student.password, student.nickname, student.group_id
    hashed = hashed_password(password)
    return db.insert_student(login=login, password=hashed, nickname=nickname, group_id=group_id)


@app.get('/students/me')
async def read_student(session=Depends(student_session)):
    return db.get_student(student_id=session['user_id'])


@app.patch('/students/me')
async def update_student(data: StudentPatch, session=Depends(student_session)):
    new_nickname, new_group_id = data.nickname, data.group_id
    student_id = session['user_id']
    if new_nickname and db.get_student_by_nickname(nickname=new_nickname):
        raise CONFLICT
    db.update_student(student_id=student_id, new_nickname=new_nickname, new_group_id=new_group_id)


@app.delete('/students/me')
async def remove_student(session=Depends(student_session)):
    student_id = session['user_id']
    db.remove_student(student_id=student_id)
    delete_session(student_sessions, session['login'])


#
# Student incoming team invitations endpoints
#

@app.get('/students/me/invitations')
async def get_student_invitations(session=Depends(student_session)):
    student_id = session['user_id']
    return db.get_student_invitations(student_id=student_id)


@app.post('/students/me/invitations/{team_id}/accept')
async def accept_team_invite(team_id: int, session=Depends(student_session)):
    student_id = session['user_id']
    if db.remove_invite(student_id=student_id, team_id=team_id) is None:
        raise FORBIDDEN
    db.change_team(student_id=student_id, team_id=team_id)


@app.post('/students/me/invitations/{team_id}/decline')
async def decline_team_invite(team_id: int, session=Depends(student_session)):
    student_id = session['user_id']
    db.remove_invite(student_id=student_id, team_id=team_id)


#
# Team endpoints
#


@app.get('/students/me/team')
async def get_student_team(session=Depends(student_session)):
    student_id = session['user_id']
    return db.get_student_team(student_id=student_id)


@app.post('/students/me/team/leave')
async def leave_student_team(session=Depends(student_session)):
    student_id = session['user_id']
    return db.remove_student_from_team(student_id=student_id)


@app.get('/team/{id}/members')
async def get_team_members(id: int, session=Depends(student_session)):
    return db.get_team_members(team_id=id)


@app.get('/team/{id}/invitations')
async def get_student_team_invitations(id: int, session=Depends(student_session)):
    return db.get_team_invitations(team_id=id)


@app.patch('/teams/{id}')
async def update_team(id: int, new_name: str = Body(..., max_length=50, embed=True), session=Depends(student_session)):
    student_id = session['user_id']
    student_team = db.get_team(team_id=id)
    if student_id != student_team['leader_id']:
        raise FORBIDDEN
    new_name = new_name.strip()
    if new_name == "":
        raise HTTPException(422, 'Cannot pass empty name!')
    elif db.get_team_by_name(name=new_name):
        raise CONFLICT
    else:
        return db.update_team_name(team_id=id, new_name=new_name)


@app.patch('/teams/{team_id}/leader/{leader_id}')
async def update_team_leader(team_id: int, leader_id: int, session=Depends(student_session)):
    student_id = session['user_id']
    student_team = db.get_team(team_id=team_id)
    if student_id != student_team['leader_id']:
        raise FORBIDDEN
    db.update_team_leader(team_id=student_team['id'], new_leader_id=leader_id)


@app.post('/teams/{team_id}/invitations/{nickname}')
async def invite_to_team(team_id: int, nickname: str, session=Depends(student_session)):
    student_id = session['user_id']
    student_team = db.get_team(team_id=team_id)
    if student_id != student_team['leader_id']:
        raise FORBIDDEN
    invited_student = db.get_student_by_nickname(nickname=nickname)
    if db.get_student_by_nickname(nickname=nickname) is None:
        raise NOT_FOUND
    if db.get_team_member(team_id=team_id, student_id=invited_student['id']) or \
            db.get_team_invitation(team_id=team_id, student_id=invited_student['id']):
        raise CONFLICT
    return db.invite_student_to_team(student_id=invited_student['id'], team_id=student_team['id'])


@app.delete('/students/{team_id}/invitations/{student_id}')
async def cancel_team_invite(team_id: int, student_id: int, session=Depends(student_session)):
    leader_id = session['user_id']
    student_team = db.get_team(team_id=team_id)
    if leader_id != student_team['leader_id']:
        raise FORBIDDEN
    db.remove_invite(student_id=student_id, team_id=team_id)


#
# Student team submissions endpoints
#

@app.get('/teams/me/submissions')
async def get_team_submissions(session=Depends(student_session)):
    student_id = session['user_id']
    team_id = db.get_student_team(student_id=student_id)['id']
    return db.get_team_submissions(team_id=team_id)


async def run_job(game_id, submission_id, ref_player_id):
    job_id = db.insert_ref_result(submission_id=submission_id, reference_id=ref_player_id, result='unknown')
    values = {"game_id": game_id,
              "p1_id": submission_id,
              "p2_id": ref_player_id,
              "is_ref": True}
    try:
        async with httpx.AsyncClient() as client:
            response = await client.put(url=f"{supervisor_api}/job/{job_id}", params=values, timeout=240)
    except httpx.HTTPError as exc:
        print(exc)
        print(f"Error while requesting {exc.request.url!r}.")
        async with httpx.AsyncClient() as client:
            response = await client.get(url=f"{supervisor_api}/job/{job_id}")
    print(response)
    response_dict = json.loads(response.text)
    print(response_dict)
    db.update_ref_result(result_id=job_id, result=response_dict['result'],
                         sub_stdout=response_dict['log']['p1'],
                         ref_stdout=response_dict['log']['p2'])


@app.post('/teams/me/submissions')
async def create_student_team_submission(is_automake: bool = Body(...),
                                         environment_id: int = Body(...),
                                         executables: UploadFile = File(...), session=Depends(student_session)):
    student_id = session['user_id']
    if not check_submission_time(student_sessions, session['login']):
        raise HTTPException(403, 'Forbidden: Only one submission in 3 minutes!')

    student_team = db.get_student_team(student_id=student_id)
    submission_id = db.insert_team_submission(is_automake=is_automake,
                                              environment_id=environment_id,
                                              team_id=student_team['id'],
                                              status='submitted')

    submission_dir = get_submission_directory(submission_id, init=True)

    try:
        save_file(submission_dir, executables, 'player', SUBMISSION_EXEC_EXT)
        db.update_team_submission_path(submission_id=submission_id, files_path=submission_dir)
        update_submission_time(student_sessions, session['login'])

        files = {"data": (
            executables.filename,
            open(f'{submission_dir}/player{os.path.splitext(executables.filename)[1]}', 'rb'))}
        values = {"env_id": str(environment_id),
                  "automake": str(is_automake)}

        async with httpx.AsyncClient() as client:
            print(f"Sending submission {submission_id}...")
            response = await client.put(url=f"{supervisor_api}/player/{submission_id}",
                                        files=files, data=values)
            print(f"Supervisor response to sent submission {submission_id}: {response.text}")

        active_game_id = db.get_active_game()['id']
        active_ref_submissions = db.get_game_ref_submissions(game_id=active_game_id)

        ref_games = [run_job(active_game_id, submission_id, ref_player['id']) for ref_player in active_ref_submissions]
        await asyncio.gather(*ref_games, return_exceptions=True)
        db.update_team_submission_status(submission_id=submission_id, status='graded')
        return submission_id
    except Exception as e:
        remove_dir(submission_dir)
        db.remove_team_submission(submission_id=submission_id)
        db.remove_ref_results(submission_id=submission_id)
        raise e


@app.post('/teams/me/submissions/primary/{id}')
async def choose_student_team_primary_submission(id: int, session=Depends(student_session)):
    student_id = session['user_id']
    student_team = db.get_student_team(student_id=student_id)
    submission = db.get_team_submission(submission_id=id)
    if submission['team_id'] != student_team['id'] or student_team['leader_id'] != student_id:
        raise FORBIDDEN
    db.set_primary_submission(team_id=student_team['id'], submission_id=id)
    return id


#
# Teacher endpoints
#

@app.post('/teachers')
async def create_teacher(teacher: TeacherPost, session=Depends(teacher_session)):
    login, password = teacher.login, teacher.password
    hashed = hashed_password(password)
    return db.insert_teacher(login=login, password=hashed)


#
# Environment endpoints
#

@app.get('/environments')
async def get_environments():
    return db.get_environments()


#
# Group endpoints
#

@app.get('/groups')
async def get_groups():
    return db.get_groups()


@app.post('/groups')
async def create_group(name: str = Body(..., max_length=20, embed=True), session=Depends(teacher_session)):
    if db.get_group_by_name(name=name):
        raise CONFLICT
    return db.insert_group(name=name)


@app.patch('/groups/{id}')
async def update_group_name(id: int, name: str = Body(..., max_length=20, embed=True),
                            session=Depends(teacher_session)):
    if db.get_group_by_name(name=name):
        raise CONFLICT
    db.update_group(group_id=id, name=name)


@app.delete('/groups/{id}')
async def remove_group(id: int, session=Depends(teacher_session)):
    db.remove_group(group_id=id)


#
# Game endpoints
#

@app.get('/games')
async def get_games(session=Depends(teacher_session)):
    return db.get_games()


@app.get('/games/active')
async def get_active_game():
    game = db.get_active_game()
    if game:
        id = game['id']
        game["overview"] = get_html_from_markdown(os.path.join(GAMES_DIR, f'{id}/overview.md'))
        game["rules"] = get_html_from_markdown(os.path.join(GAMES_DIR, f'{id}/rules.md'))
        return game
    else:
        raise NOT_FOUND


@app.get('/games/active/widget')
async def get_active_game_widget():
    game = db.get_active_game()
    if game:
        id = game["id"]
        with open(os.path.join(GAMES_DIR, f'{id}/widget.html'), "r") as input_file:
            text = input_file.read()
        return HTMLResponse(content=text, status_code=200)
    else:
        raise NOT_FOUND


@app.get('/games/{id}')
async def get_game(id: int, session=Depends(teacher_session)):
    return db.get_game(game_id=id)


@app.post('/games')
async def create_game(name: str = Body(..., max_length=50), description: str = Body(..., max_length=100),
                      environment_id: int = Body(...), deadline: date = Body(...),
                      executables: UploadFile = File(...),
                      widget: UploadFile = File(...),
                      overview: UploadFile = File(...),
                      rules: UploadFile = File(...), session=Depends(teacher_session)):
    if db.get_game_by_name(name=name):
        raise CONFLICT

    game_id = db.insert_game(name=name,
                             description=description,
                             deadline=deadline,
                             environment_id=environment_id)

    game_dir = get_game_directory(game_id, init=True)

    try:
        save_file(game_dir, widget, 'widget', WIDGET_EXT)
        save_file(game_dir, overview, 'overview', OVERVIEW_EXT)
        save_file(game_dir, rules, 'rules', RULES_EXT)
        save_file(game_dir, executables, 'judge', GAME_EXEC_EXT)

        db.update_game_path(game_id=game_id, files_path=game_dir)

        files = {"data": (
            executables.filename, open(f'{game_dir}/judge{os.path.splitext(executables.filename)[1]}', 'rb'))}
        values = {"env_id": str(environment_id)}
        async with httpx.AsyncClient() as client:
            print(f"Sending game {game_id}...")
            response = await client.put(url=f"{supervisor_api}/game/{game_id}", files=files, data=values)
            print(f"Supervisor response for game {game_id}: {response.text}")
        return game_id
    except Exception as e:
        remove_dir(game_dir)
        db.remove_game(game_id=game_id)
        raise e


@app.patch('/games/{id}')
async def update_game(id: int, name: str = Body(None, max_length=50), description: str = Body(None, max_length=100),
                      environment_id: int = Body(None), deadline: Optional[date] = Body(None),
                      widget: Optional[UploadFile] = File(None),
                      overview: Optional[UploadFile] = File(None),
                      rules: Optional[UploadFile] = File(None), session=Depends(teacher_session)):
    if name and db.get_group_by_name(name=name):
        raise CONFLICT
    db.update_game(game_id=id,
                   new_name=name,
                   new_description=description,
                   new_deadline=deadline,
                   new_environment_id=environment_id)

    game_dir = get_game_directory(id, init=True)

    if widget:
        save_file(game_dir, widget, 'widget', WIDGET_EXT)

    if overview:
        save_file(game_dir, overview, 'overview', OVERVIEW_EXT)

    if rules:
        save_file(game_dir, rules, 'rules', RULES_EXT)


@app.delete('/games/{id}')
async def remove_game(id: int, session=Depends(teacher_session)):
    game = db.get_game(game_id=id)
    if game["is_active"]:
        raise FORBIDDEN
    game_dir = get_game_directory(id)
    remove_dir(game_dir)
    db.remove_game(game_id=id)


@app.post('/games/activate/{id}')
async def activate_game(id: int, remove_students: Optional[bool] = Body(False, embed=True),
                        session=Depends(teacher_session)):
    if remove_students:
        db.remove_all_students()
        db.remove_all_teams()
        db.remove_all_groups()
    db.remove_all_team_submissions()
    db.remove_all_tournament_results()
    db.remove_all_ref_results()
    clear_dir_contents(SUBMISSIONS_DIR)
    db.deactivate_games()
    return db.activate_game(game_id=id)


@app.post('/games/{id}/ref_submissions')
async def create_game_reference_submission(id: int, name: str = Body(..., max_length=50),
                                           environment_id: int = Body(...),
                                           executables: UploadFile = File(...), session=Depends(teacher_session)):
    game_id = id
    teacher_id = session['user_id']
    submission_id = db.insert_game_ref_submission(name=name,
                                                  environment_id=environment_id,
                                                  teacher_id=teacher_id, game_id=game_id)

    submission_dir = get_game_submission_directory(game_id, submission_id, init=True)

    try:
        save_file(submission_dir, executables, 'player', SUBMISSION_EXEC_EXT)
        db.update_ref_submission_path(submission_id=submission_id, files_path=submission_dir)

        files = {"data": (
            executables.filename,
            open(f'{submission_dir}/player{os.path.splitext(executables.filename)[1]}', 'rb'))}
        values = {"env_id": str(environment_id),
                  "game_id": str(game_id)}
        async with httpx.AsyncClient() as client:
            print(f"Sending ref_submission {submission_id} for game {game_id}...")
            response = await client.put(url=f"{supervisor_api}/ref_player/{submission_id}", files=files, data=values)
            print(f"supervisor response for ref_submission {submission_id} for game {game_id}: {response.text}")

        return submission_id
    except Exception as e:
        remove_dir(submission_dir)
        db.remove_ref_submission(submission_id=submission_id)
        raise e
