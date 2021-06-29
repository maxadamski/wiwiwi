import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import make_blobs

def get_quasispherical_data(N = 600):
    X, y = make_blobs(random_state=170, n_samples=N, centers = 5)
    rng = np.random.RandomState(74)
    transformation = rng.normal(size=(2, 2))
    X = np.dot(X, transformation)
    return X
