import streamlit as st
import requests

def rerun():
    raise st.experimental_rerun()

def api(method, path):
    method = method.lower()
    url = 'http://localhost:8000' + path
    if method == 'get':
        return requests.get(url).json()
    elif method == 'post':
        return requests.post(url).json()
    
    

col1, col2 = st.columns(2)
dog = api('post', '/dog/next')
col1.image(dog['photos'][0])
col1.text(dog['mode'])

if col2.button('Like'):
    api('post', '/dog/swipe?like=true')
    rerun()

if col2.button('Dislike'):
    api('post', '/dog/swipe?like=false')
    rerun()

