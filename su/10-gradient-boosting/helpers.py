import numpy as np

class Node(object):
    def __init__(self, depth = 1, calculate_leaf_value=None):
        self.left = None
        self.right = None
        self.boundary = [None, None]  
        self.value = None
        self.depth = depth
        if calculate_leaf_value is None:
            self.calculate_leaf_value = self.mean_val_leaf
        else:
            self.calculate_leaf_value = calculate_leaf_value

    def mean_val_leaf(self, X, y, last_predicted):
        return np.mean(y)
    
    def fit(self, X, y, last_predicted, max_depth = None):
        if max_depth is not None and self.depth > max_depth:
            self.value = self.calculate_leaf_value(X, y, last_predicted)
            return
        
        best_loss = np.var(y)  # MSE
        for i in range(np.shape(X)[1]):
            candidates = np.unique(X[:, i])
            for candidate in candidates: 
                left_y, right_y = y[X[:, i] < candidate], y[X[:, i] >= candidate]
                candidate_loss = np.var(left_y)*len(left_y) + np.var(right_y)*len(right_y)
                candidate_loss /= len(y)
                if candidate_loss < best_loss:
                    self.boundary = [i, candidate]
                    best_loss = candidate_loss
                    
        if self.boundary[0] is not None:
            i, split_val = self.boundary
            self.left = Node(self.depth + 1)
            self.right = Node(self.depth + 1)
            self.left.fit(X[X[:, i] < split_val], y[X[:, i] < split_val], last_predicted[X[:, i] < split_val],max_depth)
            self.right.fit(X[X[:, i] >= split_val], y[X[:, i] >= split_val], last_predicted[X[:, i] >= split_val],max_depth)
        else:
            self.value = self.calculate_leaf_value(X, y, last_predicted)

    def predict_an_instance(self, x):           
        if self.value is not None:
            return self.value
        else:
            if x[self.boundary[0]] < self.boundary[1]:
                return self.left.predict(x)
            else:
                return self.right.predict(x)
    def predict(self, X):           
        if X.ndim == 1:
            return self.predict_an_instance(X)
        y = [self.predict_an_instance(x) for x in X]
        return np.array(y)
        
