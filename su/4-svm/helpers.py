import numpy as np
import matplotlib.pyplot as plt


def plot_classification(x, y, labels, a=None, b=None, c=None):
	if None not in [a,b,c]:
		delta = 0.025
		x_vals = np.arange(min(x), max(x), delta)
		y_vals = np.arange(min(y), max(y), delta)
		X, Y = np.meshgrid(x_vals,y_vals)
		Z = a*X+b*Y+c		
		CS = plt.contour(X, Y, Z, 6, colors='k')
		plt.clabel(CS, fontsize=9, inline=1)
	plt.scatter(x, y, c = labels)
	plt.show()


def get_separable(sklearn=False):
	dane = np.load('svm2.npz')
	x = dane['x']
	y = dane['y']
	labels = dane['label']
	if sklearn:
		return np.concatenate([x.reshape(-1,1),y.reshape(-1,1)], axis = 1), labels
	else:
		return x, y, labels


def get_non_separable(sklearn=False, N=100):
    X1 = np.random.multivariate_normal(np.array([4,4]), np.array([[2,0],[0,1]]), N)
    X2 = np.random.multivariate_normal(np.array([0,0]), np.array([[2,0],[0,1]]), N)
    X = np.concatenate((X1,X2))
    y = np.array([-1]*N + [1]*N)
    if sklearn:
        return X,y
    else:
        return X[:,0], X[:,1],y


def plotClassifier(X, y, gnb):
    plt.figure(1, figsize=(15,5))
    maxx = np.max(X,axis = 0)
    minx = np.min(X,axis = 0)
    eps = 0.5
    xx = np.linspace(minx[0]-eps, maxx[0]+eps, 100)
    yy = np.linspace(minx[1]-eps, maxx[1]+eps, 100).T
    xx, yy = np.meshgrid(xx, yy)
    Xfull = np.c_[xx.ravel(), yy.ravel()]
    prob=gnb.predict(Xfull).reshape(xx.shape)
    plt.contourf(xx, yy, prob, cmap=plt.cm.brg, alpha=0.2)
    plt.xticks(())
    plt.yticks(())
    plt.scatter(X[:,0], X[:,1], c = y)
    plt.title('Granica decyzji')

def get_wine():
    import pandas as pd
    df_r = pd.read_csv('https://archive.ics.uci.edu/ml/machine-learning-databases/wine-quality/winequality-red.csv', sep=";",na_values="?")
    df_r.dropna(inplace=True)
    df_w = pd.read_csv('https://archive.ics.uci.edu/ml/machine-learning-databases/wine-quality/winequality-white.csv', sep=";",na_values="?")
    df_w.dropna(inplace=True)
    X=pd.concat([df_r, df_w]).values
    y=np.concatenate([np.zeros(len(df_r)), np.ones(len(df_w))])
    return X,y
