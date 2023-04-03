import os, requests, PIL
import torch as T
import pandas as pd
import numpy as np

from sklearn.model_selection import train_test_split
from memenet.utils import *

test_size = 0.2
seed = 42
imgflip_prefix = 'https://i.imgflip.com'
image_vector_cache = 'data/memes/vec_img.pkl'
text_vector_cache = 'data/memes/vec_txt.pkl'
doc2vec_path = 'data/doc2vec/doc2vec_wiki_d300_n5_w8_mc50_t12_e10_dbow.model'
cache_dir = 'data/cache'

def cache_image(filename):
    dst = os.path.join(cache_dir, filename)
    if os.path.exists(dst):
        return True
    url = os.path.join(imgflip_prefix, filename)
    res = requests.get(url)
    if res.status_code != 200: return False
    with open(os.path.join(cache_dir, filename), 'wb') as f:
        f.write(res.content)
    return True

def load_image(filename):
    path = os.path.join(cache_dir, filename)
    if not cache_image(filename): return None
    return PIL.Image.open(path)

def load_dataset():
    assert os.path.exists('data/memes')

    memes = pd.read_csv('data/memes/memes_data.tsv', sep='\t')
    memes.ImageURL = memes.ImageURL.str.replace('//i.imgflip.com/', '', regex=False)
    memes.drop(columns=['AltText', 'HashId'], inplace=True)
    memes.columns = ['caption', 'file', 'template']

    meme_templates = pd.read_csv('data/memes/memes_reference_data.tsv', sep='\t')
    meme_templates.BaseImageURL = meme_templates.BaseImageURL.str.replace('https://i.imgflip.com/', '', regex=False)
    meme_templates.drop(columns=['Height', 'Width', 'StandardTextBox'], inplace=True)
    meme_templates.columns = ['template', 'template_file']

    memes_joined = pd.merge(memes, meme_templates, on='template')

    idx_tr, idx_te = train_test_split(memes_joined.index, stratify=memes_joined.template, test_size=test_size, random_state=seed)
    memes_joined['set'] = 'train'
    memes_joined.loc[idx_te, 'set'] = 'test'

    memes_joined.to_csv('data/memes/memes.csv', index=False)
    meme_templates.to_csv('data/memes/templates.csv', index=False)

    return memes_joined, meme_templates

def load_image_vectors():
    if os.path.exists(image_vector_cache):
        return unpickle(image_vector_cache)
    print('caching image embeddings')

    from img2vec_pytorch import Img2Vec
    img2vec = Img2Vec(cuda=cuda)
    filenames = meme_templates.template_file
    images = [load_image(filename) for filename in filenames]
    vectors = img2vec.get_vec(images)
    for image in images: image.close()
    result = {filename: T.tensor(vector) for filename, vector in zip(filenames, vectors)}
    pickle(result, to=image_vector_cache)
    return result

def load_text_vectors():
    assert os.path.exists(doc2vec_path)
    if os.path.exists(text_vector_cache):
        return unpickle(text_vector_cache)
    print('caching caption embeddings')

    from gensim.models.doc2vec import Doc2Vec
    from gensim.utils import tokenize
    from tqdm import tqdm
    doc2vec = Doc2Vec.load(doc2vec_path)
    result = {caption: T.tensor(doc2vec.infer_vector(tokenize(caption))) for caption in tqdm(memes.caption)}
    pickle(result, to=text_vector_cache)
    return result

if not os.path.exists(cache_dir):
    os.mkdir(cache_dir)

if os.path.exists('data/memes/memes.csv') and os.path.exists('data/memes/templates.csv'):
    memes, meme_templates = pd.read_csv('data/memes/memes.csv'), pd.read_csv('data/memes/templates.csv')
else:
    memes, meme_templates = load_dataset()
