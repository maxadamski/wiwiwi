import numpy as np
import matplotlib.pyplot as plt
import itertools
from collections import defaultdict
from  scipy.stats import multivariate_normal


# UWAGA! Lektura tego pliku NIE powinna być konieczna, aby rozwiązać ćwiczenia

def generate1(N=100):
    X1 = np.random.multivariate_normal(np.array([4,4]), np.array([[2,0],[0,1]]), N)
    X2 = np.random.multivariate_normal(np.array([0,0]), np.array([[2,0],[0,1]]), N)
    X = np.concatenate((X1,X2))
    y = np.array([0]*N + [1]*N)
    return X,y

def generate2(N=100):
    X1 = np.random.multivariate_normal(np.array([4,4]), np.array([[5,-4],[-4,4]]), N)
    X2 = np.random.multivariate_normal(np.array([0,0]), np.array([[5,-4],[-4,4]]), N)
    X = np.concatenate((X1,X2))
    y = np.array([0]*N + [1]*N)
    return X,y

def generate3(N=100):
    X1 = np.random.multivariate_normal(np.array([3,0.5]), np.array([[5,-4],[-4,4]]), N)
    X2 = np.random.multivariate_normal(np.array([0,0]), np.array([[5,-4],[-4,4]]), N)
    X = np.concatenate((X1,X2))
    y = np.array([0]*N + [1]*N)
    return X,y

SUM = "suma"
class FullBayes():
    def __init__(self):
        self.prob = {}
        self.class_log_prob = None
    
    def _hash_example(self, x):
        return tuple(x.tolist())
    
    def fit(self, X, y):
        unique, counts = np.unique(y, return_counts=True)
        self.class_log_prob = np.log(counts/np.sum(counts))
        for i, clazz in enumerate(y):
            if clazz not in self.prob:
                if X.shape[1]>10:
                    self.prob[clazz] = defaultdict(int)
                else:
                    self.prob[clazz] = {}
                    # Powyższe wywołanie lepiej by było zastąpić defaultdict(int)
                    # i usunięcie wpisywania zer - jednak chodzi o to by student
                    # mógł zobaczyć wszystkie estymaty (i potencjalnie sporo zer)
                    for j in itertools.product([0,1], repeat= X.shape[1]):
                        self.prob[clazz][j]= 0
                    self.prob[clazz][SUM]= 0

            self.prob[clazz][self._hash_example(X[i])] +=1
            self.prob[clazz][SUM] += 1
        
    def predict_proba(self, X):
        results = np.tile(self.class_log_prob, (X.shape[0], 1))
        for i in range(X.shape[0]):
            for j in range(len(self.class_log_prob)):
                results [i,j] += np.nan_to_num(np.log(self.prob[j][self._hash_example(X[i])] /self.prob[j][SUM] ))
        probabilities = np.exp(results)
        probabilities /= np.sum(probabilities, axis=1, keepdims=True)
        return probabilities
    
    def predict(self,X):
        prob = self.predict_proba(X)
        return np.argmax(prob, axis=1)

class SmoothFullBayes(FullBayes):               
    def fit(self, X, y):
        unique, counts = np.unique(y, return_counts=True)
        self.class_log_prob = np.log(counts/np.sum(counts))
        for i, clazz in enumerate(y):
            if clazz not in self.prob:
                if X.shape[1]>10:
                    self.prob[clazz] = defaultdict(lambda : 1)
                    self.prob[clazz][SUM]= 2**X.shape[1] 
                else:
                    self.prob[clazz] = {}
                    # Powyższe wywołanie lepiej by było zastąpić defaultdict(int)
                    # i usunięcie wpisywania zer - jednak chodzi o to by student
                    # mógł zobaczyć wszystkie estymaty (i potencjalnie sporo zer)
                    for j in itertools.product([0,1], repeat= X.shape[1]):
                        self.prob[clazz][j]= 1
                    self.prob[clazz][SUM]= len(self.prob[clazz])

            self.prob[clazz][self._hash_example(X[i])] +=1
            self.prob[clazz][SUM] += 1


class GaussianBayes():
    def __init__(self):
        self.means = {}
        self.stds = {}
        self.class_log_prob = None
    def fit(self, X, y):
        unique, counts = np.unique(y, return_counts=True)
        self.class_log_prob = np.log(counts/np.sum(counts))
        for clazz in unique:
            self.means[clazz] = np.mean(X[y == clazz], axis=0)
            self.stds[clazz] = np.cov(X[y == clazz].T)
        
    def predict_proba(self, X):
        results = np.tile(self.class_log_prob, (X.shape[0],1))
        for i in range(len(self.class_log_prob)):
            results [:,i] += multivariate_normal.logpdf(X, self.means[i], self.stds[i])
        probabilities = np.exp(results)
        probabilities /= np.sum(probabilities, axis=1, keepdims=True)
        return probabilities
    
    def predict(self,X):
        prob = self.predict_proba(X)
        return np.argmax(prob, axis=1)


def plotGaussianBayes(X, y, gnb):
    plt.figure(1, figsize=(15,5))
    plt.subplot(121)

    maxx = np.max(X,axis = 0)
    minx = np.min(X,axis = 0)
    eps = 0.5
    xx = np.linspace(minx[0]-eps, maxx[0]+eps, 100)
    yy = np.linspace(minx[1]-eps, maxx[1]+eps, 100).T
    xx, yy = np.meshgrid(xx, yy)
    Xfull = np.c_[xx.ravel(), yy.ravel()]
    prob=gnb.predict_proba(Xfull)
    imshow_handle = plt.imshow(prob[:, 0].reshape((100, 100)),
                                   extent=(minx[0]-eps, maxx[0]+eps, minx[1]-eps, maxx[1]+eps), origin='lower')
    plt.xticks(())
    plt.yticks(())
    plt.scatter(X[:,0], X[:,1], c = y)
    plt.title('Estimated cond. probability')

    plt.subplot(122)    

    colors = np.array(['red','blue'])
    for i in range(2):
        if len(gnb.stds[0].shape) <2:
            Z = multivariate_normal.pdf(Xfull, gnb.means[i], gnb.stds[i]@np.eye(2,2))
        else: 
            Z = multivariate_normal.pdf(Xfull, gnb.means[i], gnb.stds[i])
        CS = plt.contour(xx, yy, Z.reshape((100,100)), 6, colors = colors[i])
        plt.clabel(CS, fontsize=9, inline=1)
    plt.scatter(X[:,0], X[:,1], c = colors[y])
    plt.title('Estimated Gaussians for each class')
    plt.show()

def generate_binary(N, k=50):
    p = np.random.rand(k)
    X = np.random.binomial(1,p, size = (N,k))
    w = np.random.normal(0,10,k)
    y = (1/(1+np.exp(-X@w+ (k/25))) > 0.5).astype(int)
    # Nawet zbiór 2-elementowy zawiera wszystkie klasy
    y[0] = 0
    y[1] = 1
    return X, y

def plotAccuracyIterationsPlot(iterations, results, results_train):
    plt.figure(1, figsize=(15,5))
    plt.subplot(121)
    for name, result in results.items():
        plt.plot(iterations,result, label=name)
    plt.legend()
    plt.title("Trafność na zbiorze testowym")

    plt.subplot(122)
    for name, result in results_train.items():
        plt.plot(iterations, result, label=name)
    plt.title("Trafność na zbiorze uczącym")
    plt.legend()
    plt.show()
