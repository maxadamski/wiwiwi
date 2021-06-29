CREATE KEYSPACE IF NOT EXISTS embedb
  WITH REPLICATION = {'class': 'SimpleStrategy', 'replication_factor': 3 };

USE embedb;

DROP TABLE IF EXISTS measurements;

CREATE TABLE measurements (
	sensor_id int,
	stamp     timestamp,
	value     double,
	PRIMARY KEY (sensor_id, stamp)
) WITH default_time_to_live = 8640000;

