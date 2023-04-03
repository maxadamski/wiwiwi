import os
import shutil
import markdown
import toml

from typing import IO
from tempfile import NamedTemporaryFile
from fastapi import HTTPException
from pathlib import Path

FILES_DIR = os.path.join(os.getcwd(), 'files')
GAMES_DIR = os.path.join(FILES_DIR, 'games')
SUBMISSIONS_DIR = os.path.join(FILES_DIR, 'submissions')

BAD_SIZE = HTTPException(413, 'Entity Too Large')
BAD_EXTENSION = HTTPException(415, 'Wrong File Extension')

config = toml.load('config.toml')
MB_LIMIT = config['files']['mb_limit']
WIDGET_EXT = config['files']['widget_ext']
OVERVIEW_EXT = config['files']['overview_ext']
RULES_EXT = config['files']['rules_ext']
GAME_EXEC_EXT = config['files']['game_exec_ext']
SUBMISSION_EXEC_EXT = config['files']['submission_exec_ext']

def read_file_limited(file):
    real_file_size = 0
    temp: IO = NamedTemporaryFile(delete=False)
    for chunk in file.file:
        real_file_size += len(chunk)
        if real_file_size > MB_LIMIT * 1000000:
            os.unlink(temp.name)
            raise BAD_SIZE
        temp.write(chunk)
    temp.close()
    return temp


def move_temp_file(file, path):
    shutil.move(file.name, path)


def clear_dir_contents(directory):
    for root, dirs, files in os.walk(directory):
        for f in files:
            os.unlink(os.path.join(root, f))
        for d in dirs:
            shutil.rmtree(os.path.join(root, d))


def remove_dir(directory):
    shutil.rmtree(directory, ignore_errors=True)


def get_game_directory(game_id, init=False):
    game_dir = os.path.join(GAMES_DIR, str(game_id))
    if init:
        Path(game_dir).mkdir(parents=True, exist_ok=True)
    return game_dir


def get_game_submission_directory(game_id, submission_id, init=False):
    game_submission_dir = os.path.join(GAMES_DIR, str(game_id), 'submissions', str(submission_id))
    if init:
        Path(game_submission_dir).mkdir(parents=True, exist_ok=True)
    return game_submission_dir


def get_submission_directory(submission_id, init=False):
    submission_dir = os.path.join(SUBMISSIONS_DIR, str(submission_id))
    if init:
        Path(submission_dir).mkdir(parents=True, exist_ok=True)
    return submission_dir


def save_file(directory, file, name, accept_ext):
    ext = os.path.splitext(file.filename)[1]
    if ext in accept_ext:
        temp_path = read_file_limited(file)
        location = os.path.join(directory, name + ext)
        move_temp_file(temp_path, location)
    else:
        raise BAD_EXTENSION


def get_html_from_markdown(markdown_path):
    with open(markdown_path, "r") as input_file:
        text = input_file.read()
    html = markdown.markdown(text)
    return html
