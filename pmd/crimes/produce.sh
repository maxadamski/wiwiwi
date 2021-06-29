#!/bin/sh

java -jar TestProducer.jar \
	--mode generate \
	--bootstrap localhost:9092 \
	--topic big-data \
	--ts_format "yyyy-MM-dd'T'HH:mm:ss.SSSX" \
	--ts_col 1 \
	--skip 1 \
	--delay 10 \
	--input $1
