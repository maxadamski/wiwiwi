# How to run files in `memenet`

1. Install PyTorch however you want
2. Install requirements `pip install -r requirements.txt`
3. Download doc2vec [here](https://github.com/kongyq/Pretrained_Wikipedia_Doc2Vec_Models) and place files in `data/doc2vec/`
4. Download the dataset [here](https://www.kaggle.com/datasets/abhishtagatya/imgflipscraped-memes-caption-dataset) and place files in `data/memes/`
6. Train MemeNet by running `python -m memenet.train` in the same directory as `README.md`
7. Run streamlit app `streamlit run app.py`

If something is not working, first ensure that the `data/` directory looks like this:

```
data
├── doc2vec
│   ├── doc2vec_wiki_d300_n5_w8_mc50_t12_e10_dbow.model
│   ├── doc2vec_wiki_d300_n5_w8_mc50_t12_e10_dbow.model.docvecs.vectors_docs.npy
│   ├── doc2vec_wiki_d300_n5_w8_mc50_t12_e10_dbow.model.trainables.syn1neg.npy
│   └── doc2vec_wiki_d300_n5_w8_mc50_t12_e10_dbow.model.wv.vectors.npy
└── memes
    ├── memes_data.tsv
    └── memes_reference_data.tsv
```

# GPU Configuration

- CUDA: `conda install -c anaconda cudatoolkit=11.3`
- cudnn `conda install -c anaconda cudnn=8.2.1=cuda11.3_0`

