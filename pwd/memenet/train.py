import numpy as np
import torch as T

import memenet
from memenet import memes, meme_templates
from memenet.models import ImgNet, TxtNet
from memenet.utils import *

meme_loss = T.nn.TripletMarginLoss()

def distance(a, b):
    return ((a - b)**2).sum(1)

def meme_train(*, img_net, txt_net, memes, img_emb, txt_emb, epochs=6, batch_size=32, lr=1e-4):
    history = []
    optim = T.optim.AdamW(txt_net.parameters(), lr=lr)
    train_memes, test_memes = memes[memes.set == 'train'], memes[memes.set == 'test']
    templates = list(train_memes.template_file.unique())
    n_samples = len(train_memes)
    for epoch in range(epochs):
        txt_net.train()
        train_losses = []
        batches = minibatches(T.randperm(n_samples), batch_size)
        for batch_idx, batch in enumerate(batches):
            Y_sim = T.stack([img_emb[name] for name in train_memes.iloc[batch].template_file])
            Y_dis = T.stack([img_emb[random_choice(templates, exclude=name)] for name in train_memes.iloc[batch].template_file])
            X_txt = T.stack([txt_emb[caption] for caption in train_memes.iloc[batch].caption])
            Y_txt = txt_net(X_txt)
            
            loss = meme_loss(Y_txt, Y_sim, Y_dis)
            loss.backward()
            optim.step()
            optim.zero_grad()
            train_losses.append(loss.item())
            train_loss = np.mean(train_losses)
            prefix = f'\repoch {prog(epoch+1, epochs)} | batch {prog(batch_idx+1, len(batches))} | train loss {train_loss:.4f}'
            print(f'{prefix}', end='')
            
        txt_net.eval()
        with T.no_grad():
            Y_te_img = T.stack([img_emb[filename] for filename in templates])
            X_te_txt = T.stack([txt_emb[caption] for caption in test_memes.caption])
            Y_te_txt = txt_net(X_te_txt)
            
            acc = []
            for txt_index, vector in enumerate(Y_te_txt):
                img_indices = T.argsort(distance(vector, Y_te_img))
                y = test_memes.iloc[txt_index].template_file[0]
                i = 1
                for img_index in img_indices:
                    if templates[img_index] == y:
                        break
                    i += 1
                acc.append(1 / i)
            acc = np.mean(acc)
                
        history.append(dict(epoch=epoch, train_loss=train_loss, hits=acc))
        print(f'{prefix} | discounted accuracy {acc:.4f} (test)')
        
    return history

if __name__ == '__main__':
    img_emb = memenet.dataset.load_image_vectors()
    txt_emb = memenet.dataset.load_text_vectors()

    img_net = ImgNet()
    txt_net = TxtNet(hidden_dim=128, output_dim=512)
    print('ImgNet param count:', param_count(img_net))
    print('TxtNet param count:', param_count(txt_net))

    meme_train(img_net=img_net, txt_net=txt_net, memes=memes, img_emb=img_emb, txt_emb=txt_emb)
