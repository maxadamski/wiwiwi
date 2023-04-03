import math

import pandas as pd
from sent2vec.vectorizer import Vectorizer
from ai.datasets import loader

list_of_words = loader.get_captions_sentences_as_list()

vectorizer = Vectorizer(pretrained_weights='distilbert-base-multilingual-cased')

list_size = len(list_of_words)

splits = 150

for x in range(splits):
    start = math.floor((list_size/splits)*x)
    stop = math.floor((list_size/splits)*(1+x))
    print(start, stop)
    vectorizer.run(list_of_words[start:stop])
vectors = vectorizer.vectors
pd.to_pickle(vectors, 'sent2vec.pkl')
