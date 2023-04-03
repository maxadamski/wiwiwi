import sys
import streamlit as st
import rapidfuzz
from memenet import dataset, memes, ImgNet, TxtNet
from memenet.utils import * 

memes.drop_duplicates('caption')

def image_url(filename):
    return dataset.imgflip_prefix + '/' + filename

def main():
    st.title('MemeNet 😂')
    query = st.text_input('Try writing something')

    if not query: return

    _, scores, indices = unzip(rapidfuzz.process.extract(query, memes.caption, limit=5))
    m = memes.iloc[indices].copy()
    m['score'] = scores

    st.subheader('Suggested memes')
    for col, filename in zip(st.columns(5), m.file):
        col.image(image_url(filename))

    st.subheader('Matching templates')
    for col, filename in zip(st.columns(5), m.template_file.unique()):
        col.image(image_url(filename))

    st.subheader('Detailed ranking')
    st.dataframe(m[['caption', 'template', 'file', 'template_file']].reset_index(drop=True))

main()
