redis:
	redis-server --port 6666 &
	redis-server --port 6667 &

devel:
	uvicorn main:app --reload --timeout-keep-alive 240

serve:
	uvicorn main:app
