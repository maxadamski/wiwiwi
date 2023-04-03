# psinder

Psinder

UI demo: (https://streamable.com/mjnxv3)

## How to run

Install Python (>=3.8) and TensorFlow in any way you like (for example, with Conda).

Install other dependencies:

```
pip install -r requirements.txt
```

Run server:

```
scripts/serve.sh
```

Interactive API docs are available at `localhost:8000/docs`.

## How to make requests with session token

1. Make a `POST` request on `/login` with a username and password.
	- You will get a session token in response (valid for the next 24 hours or until server restart)
	- Test login: `tester`, test password: `helloworld`
2. In requests requiring authentication, set the following HTTP headers:
	- `x-session-username` to your username
	- `x-session-token` to your session token

## How to play with user preferences

1. Install development requirements `pip install -r requirements-dev.txt`
2. Set `debug_endpoints = true` in `config.toml`
3. Start the server
4. Start preference explorer `streamlit run scripts/bandit.py`

## Authors

Sławek Gilewski, Max Adamski
