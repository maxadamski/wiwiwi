import os
import secrets
from datetime import datetime, timedelta
from passlib.hash import argon2, sha256_crypt

def hashed_password(password: bytes) -> bytes:
    return argon2.using(rounds=4, salt_size=128).hash(password)

def verify_password(password: bytes, hash: bytes) -> bool:
    try:
        return argon2.verify(password, hash)
    except:
        return False

def hashed_token(token: bytes) -> bytes:
    return sha256_crypt.using(rounds=1000).hash(token)

def verify_token(token: bytes, hash: bytes) -> bytes:
    try:
        return sha256_crypt.verify(token, hash)
    except:
        return False

def make_password_reset_token() -> str:
    return secrets.token_urlsafe(32)

def make_session_token() -> str:
    return secrets.token_urlsafe(64)

def utcnow() -> datetime:
    return datetime.utcnow()

def utcfuture(**delta) -> datetime:
    return utcnow() + timedelta(**delta)

def fastpop(xs, i):
    # Remove `i`-th element from `xs` in O(1) time.
    # Use only if element order in `xs` doesn't matter.
    xs[i], xs[-1] = xs[-1], xs[i]
    return xs.pop(-1)

def is_sorted(xs):
    return all(xs[i] <= xs[i+1] for i in range(len(xs) - 1))

def fastpop_all(xs, indices):
    assert is_sorted(indices)
    res = []
    for i in reversed(indices):
        res.append(fastpop(xs, i))
    return res

def partition(xs, i):
    return xs[:i], xs[i:]
