from datetime import datetime, timedelta


def utcnow() -> datetime:
    return datetime.utcnow()


def utcfuture(**delta) -> datetime:
    return utcnow() + timedelta(**delta)
