#!/bin/sh

API=http://localhost:8080

# sensor 42 reports measurement 0.1234
curl -XPOST "$API/sensor/42" --data 0.1234

# get all sensor measurements between dates [from, to)
# default date format yyyy-MM-dd can be changed with ?dateformat=...
curl -XGET "$API/sensor/42?from=2021-06-01&to=2021-06-30"
echo

# get measurement aggregates in window [from, to)
curl -XGET "$API/sensor/42/agg?from=2021-06-01&to=2021-06-30"
echo

