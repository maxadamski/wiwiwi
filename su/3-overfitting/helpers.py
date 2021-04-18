from sklearn.metrics import mean_squared_error
import numpy as np
import matplotlib.pyplot as plt
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression

def true_fun(X):
    return np.cos(1.5 * np.pi * X)

def true_poly10(X):
    x_def = np.array([-1,0,1,2,3,5,7,9,8,-3])
    y_def = np.array([-1,3,2.5,5,4,2,5,4,-15,10])
    polynomial_features = PolynomialFeatures(degree=10, include_bias=True)
    x_def = polynomial_features.fit_transform(x_def.reshape(-1,1))
    lr = LinearRegression()
    lr.fit(x_def,y_def)
    w = lr.coef_
    x = polynomial_features.fit_transform(X.reshape(-1,1))
    return x@w

def plot_poly10(X, y, regresor=None, degree=None):
    return plot_poly_generic(X, y, regresor=regresor, degree=degree, func=true_poly10, \
                             x_range=(-5,10), y_range=(-20,20))

def plot_poly(X, y, regresor=None, degree=None, func=true_fun):
    return plot_poly_generic(X, y, regresor=regresor, degree=degree, func=true_fun)

def plot_poly_generic(X, y, regresor=None, degree=None, func=true_fun, x_range=(0,1), y_range=(-2, 2)):
    x_from, x_to = x_range
    X_test = np.linspace(x_from, x_to, 100)
    if regresor is not None:
        plt.plot(X_test, regresor.predict(X_test.reshape(-1,1)), label="Model")
    plt.plot(X_test, func(X_test), label="Prawdziwa funkcja")
    plt.scatter(X, y, edgecolor='b', s=20, label="Obserwacje")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.xlim((x_from, x_to))
    plt.ylim(y_range)
    plt.legend(loc="best")
    if degree is not None:
        plt.title("Stopień {}\n MSE: {:.2e}".format(degree, mean_squared_error(y, regresor.predict(X))))

def get_data(n_samples=30, seed = 0, func=true_fun, min_x=0, max_x=1, noise=0.1):
    np.random.seed(seed)
    X = np.sort(np.random.rand(n_samples)) *(max_x-min_x) +min_x
    y = func(X) + np.random.randn(n_samples) * noise
    X = X.reshape(-1,1)
    return X,y

def get_data_poly10(n_samples = 15, seed =0 ):
    return get_data(func=true_poly10, max_x=9, min_x= -3, noise=2, n_samples=n_samples, seed=seed)

def get_breast_cancer():
    import pandas as pd
    from sklearn.preprocessing import LabelEncoder
    df = pd.read_csv('https://archive.ics.uci.edu/ml/machine-learning-databases/breast-cancer/breast-cancer.data',header=None, na_values="?")
    df.dropna(inplace=True)
    df = df.apply(LabelEncoder().fit_transform)
    y = df[0]
    X = df.drop(columns=[0])
    return X.values, y.values
