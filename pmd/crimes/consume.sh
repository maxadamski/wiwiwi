#!/bin/sh

case $1 in
	s|stat)
		echo "month,district,category,total,total_arrest,total_domestic,total_fbi"
		cat stat/*.csv | sort
		;;
	a|anom)
		echo "start,end,district,total,total_fbi,fbi_pr,anomaly"
		cat anom/*.csv | sort
		;;
	*)
		echo "usage: ./consume.sh (anom|stat)"
		exit 1
		;;
esac
