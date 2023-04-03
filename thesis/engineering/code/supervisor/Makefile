.PHONY: redis devel serve

redis:
	#redis-server --port 7777 & disown

devel:
	mkdir -p files/games files/players
	uvicorn main:app --reload --port 8001

serve:
	mkdir -p files/games files/players
	gunicorn -b 0.0.0.0:8001 -w 4 -k uvicorn.workers.UvicornWorker --preload main:app
