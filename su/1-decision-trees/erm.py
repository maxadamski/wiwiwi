# Autor: Dariusz Max Adamski
# Miara: min(rmse(mean(left), y), rmse(mean(right), y)), przy splicie na left i right
# Ograniczenia (dobierane eksperymentalnie):
# - max_depth: max głębokość drzewa
# - min_leaf_samples: min odsetek przykładów o równej (podobnej wg eps) wartości y przy którym drzewo tworzy liść
# - min_samples_leaf: min ilość przykładów w każdym poddrzewie
# - eps: max odchylenie od średniej wartości y, które jest uważane za pomijalne

import pandas as pd
import numpy as np
import os.path as path
import numba
from tqdm import tqdm

def read_dataset(id, root='data'):
    X_tr = pd.read_csv(path.join(root, f'{id}-X.csv')).values
    y_tr = pd.read_csv(path.join(root, f'{id}-Y.csv')).values
    X_te = pd.read_csv(path.join(root, f'{id}-test.csv')).values
    return X_tr, y_tr[:,0], X_te, None

@numba.njit
def rmse(Y, y):
    return np.sqrt(np.mean((Y - y)**2))

@numba.njit
def best_split(X, y, min_samples_leaf=1):
    # Ugly imperative code for Numba to optimize
    n_rows, n_cols = X.shape
    loss, split = None, (None, None)
    cols = np.arange(n_cols)
    for col in cols:
        vals = np.unique(X[:,col])
        for val in vals:
            mask = X[:,col] < val
            a, b = y[mask], y[~mask]
            if len(a) < min_samples_leaf or len(b) < min_samples_leaf:
                continue
            new_loss = min(rmse(np.mean(a), y), rmse(np.mean(b), y))
            if loss is None or loss < new_loss:
                loss = new_loss
                split = col, val
    return split

class Node:
    def __init__(self):
        self.lhs, self.rhs, self.leaf, self.split = [None]*4
    
    def set_params(self, params):
        for k, x in params.items(): setattr(self, k, x)
        self.params = params
        
    def extend(self):
        tree = Node()
        tree.set_params(self.params)
        return tree
    
    def _fit(self, X, y, depth=0):
        n_rows = len(y)
        mean_y = np.mean(y)
        n_matches = np.sum(np.abs(mean_y - y) < self.eps)
        if self.max_depth == depth or n_matches/n_rows > self.min_leaf_samples:
            self.leaf = mean_y
            return
        
        self.split = col, val = best_split(X, y, min_samples_leaf=self.min_samples_leaf)
        if col is None:
            self.leaf = mean_y
            return
        
        mask = X[:,col] < val
        self.lhs, self.rhs = self.extend(), self.extend()
        self.lhs._fit(X[ mask], y[ mask], depth=depth+1)
        self.rhs._fit(X[~mask], y[~mask], depth=depth+1)
        
    def fit(self, X, y, **params):
        self.set_params(params)
        self._fit(X, y, depth=0)
    
    def predict_one(self, x):
        if self.leaf is not None: return self.leaf
        col, val = self.split
        node = self.lhs if x[col] < val else self.rhs
        return node.predict_one(x)
    
    def predict(self, X):
        return np.array([self.predict_one(X[i,:]) for i in range(X.shape[0])])
    
    def depth(self):
        return 0 if self.leaf is not None else 1 + max(self.lhs.depth(), self.rhs.depth())

if __name__ == '__main__':
    default_params = dict(max_depth=8, min_leaf_samples=0.5, min_samples_leaf=1, eps=0.01)
    best_params = dict()
    for id in tqdm(range(1, 14)):
        X_tr, y_tr, X_te, _ = read_dataset(id=id, root='data')
        params = best_params.get(id, default_params)
        tree = Node()
        tree.fit(X_tr, y_tr, **params)
        Y_te = tree.predict(X_te)
        pd.DataFrame(Y_te, columns=['Y']).to_csv(f'data/{id}.csv', index=None)

