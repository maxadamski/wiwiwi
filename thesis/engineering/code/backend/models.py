import pydantic

from pydantic import BaseModel, Field
from typing import Optional


def check_empty_string(v: str) -> str:
    if isinstance(v, str):
        s = v.strip()
        if s != "":
            return s
        else:
            raise ValueError('Cannot pass empty String')
    return v


def string_strip_validator(field: str) -> classmethod:
    decorator = pydantic.validator(field, allow_reuse=True)
    validator = decorator(check_empty_string)
    return validator


class UserLogin(BaseModel):
    login: str = Field(..., max_length=50)
    password: str = Field(..., max_length=50)

    check_strings: classmethod = string_strip_validator('*')


class StudentPost(BaseModel):
    login: str = Field(..., max_length=50)
    password: str = Field(..., max_length=50)
    nickname: str = Field(..., max_length=50)
    group_id: Optional[int] = None
    check_strings: classmethod = string_strip_validator('*')


class StudentPatch(BaseModel):
    nickname: Optional[str] = Field(None, max_length=50)
    group_id: Optional[int] = None

    check_strings: classmethod = string_strip_validator('*')


class TeacherPost(BaseModel):
    login: str = Field(..., max_length=50)
    password: str = Field(..., max_length=50)

    check_strings: classmethod = string_strip_validator('*')
