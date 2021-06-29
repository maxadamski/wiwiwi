#!/bin/sh

# argumenty jar: <okno (dni)> <%fbi> <stat_trigger> <anom_trigger> <temat> <serwer> <plik_iucr>

spark-submit \
	--packages org.apache.spark:spark-sql-kafka-0-10_2.12:3.1.2 \
	--driver-memory 8g \
	--class Main --master local[*] target/scala-2.12/big-data_2.12-0.1.0.jar \
	7 10 "10 seconds" "10 seconds" big-data localhost:9092 iucr.csv
