#!/bin/sh

gunicorn $@ --preload --worker-class=uvicorn.workers.UvicornWorker src.main:api
