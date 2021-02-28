import cv2 as cv
import pandas as pd
import numpy as np
import pickle
import os
import seaborn as sns
import altair as alt
import matplotlib.pyplot as plt
import keras

from glob import glob

from sklearn.model_selection import train_test_split
from sklearn.metrics import confusion_matrix
from sklearn.preprocessing import OneHotEncoder

from keras.models import Sequential
from keras.layers import  *
from keras.callbacks import *
from keras.optimizers import Adam
from keras.preprocessing.image import ImageDataGenerator

from cvtools import *

labels = [x.strip().split(' ')[0] for x in open('dobble.txt').readlines()]
enc = OneHotEncoder()
enc.fit(np.array(labels).reshape(-1, 1))

def decode_Y(Y):
    return enc.inverse_transform(Y).reshape(-1)

def encode_Y(Y):
    return enc.transform(Y.reshape(-1, 1)).todense()

def predict_cnn(model, X):
    Y = model.predict(X)
    return decode_Y(Y)

def load_dataset(get_features, base_path='res/sup'):
    X, y, z = [], [], []
    for label in os.listdir(base_path):
        for file in glob(os.path.join(base_path, f'{label}/*.png')):
            img = cv.imread(file)
            X.append(get_features(img))
            y.append(label)
            z.append(file)
    X, y, z = [np.array(x) for x in [X, y, z]]
    print(f'X: {X.shape}, y: {y.shape}')
    return X, y, z

def make_model(n_classes, input_shape):
    model = Sequential()
    model.add(Conv2D(126, (3, 3), activation='relu', input_shape=input_shape))
    model.add(Conv2D(126, (3, 3), activation='relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Dropout(0.2))
  
    model.add(Conv2D(256, (3, 3), activation='relu'))
    model.add(Conv2D(256, (3, 3), activation='relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Dropout(0.2))
  
    model.add(Flatten())
    model.add(Dense(256, activation='relu'))
    model.add(Dropout(0.2))
    model.add(Dense(128, activation='relu'))
    model.add(Dropout(0.2))
    model.add(Dense(n_classes, activation='softmax'))
    return model

def make_dobble_model():
    model = make_model(n_classes=59, input_shape=(40, 40, 3))
    optim = Adam(lr=1e-3)
    model.compile(optimizer=optim, loss='categorical_crossentropy', metrics=['accuracy'])
    model.summary()
    return model

def data_split(X, y):
    X = X.reshape(-1, 40, 40, 3)
    X_tr, X_te, y_tr, y_te = train_test_split(
        X, y, test_size=0.3, stratify=y, random_state=42)
    y_tr = encode_Y(y_tr)
    y_te = encode_Y(y_te)
    return X_tr, X_te, y_tr, y_te

def train_model(model, X, y, epochs=100, batch_size=32, save_path=''):
    X_tr, X_te, y_tr, y_te = data_split(X, y)

    tr = ImageDataGenerator(
        featurewise_center=True,
        featurewise_std_normalization=True,
        rotation_range=360,
        brightness_range=[0.4, 1.0],
        #shear_range=30,
    )
    tr.fit(X_tr)

    te = ImageDataGenerator(
        featurewise_center=True,
        featurewise_std_normalization=True,
    )
    te.fit(X_te)


    reduce_lr = ReduceLROnPlateau(factor=0.5, patience=3, min_lr=1e-5, monitor='val_loss', mode='min', verbose=1)
    checkpoint = ModelCheckpoint(save_path, save_best_only=True, monitor='val_loss', verbose=1)
    early_stop = EarlyStopping(patience=10, monitor='val_loss', verbose=1)
    callbacks = [reduce_lr, early_stop]
    if save_path: callbacks.append(checkpoint)

    history = model.fit_generator(
        tr.flow(X_tr, y_tr, batch_size=batch_size),
        steps_per_epoch=len(X_tr) / batch_size,
        epochs=epochs,
        validation_data=te.flow(X_te, y_te, batch_size=batch_size),
        validation_steps=len(X_te) / batch_size,
        callbacks=callbacks,
        verbose=1
    )
    
    return history
    
def plot_confusion_matrix(cm, labels=None):
    plt.figure(figsize=(20, 16))
    N = cm.shape[0]
    if labels is None: labels = list(range(N))
    df = pd.DataFrame(cm, labels, labels)
    sns.heatmap(df, annot=True, annot_kws=dict(size=10))
    plt.show()
    
def load_model(path):
    return keras.models.load_model(path)

def save_model(model, path):
    model.save(path)

def plot_loss(history):
    sns.set_style("whitegrid")
    plt.figure(figsize=(7, 4.5))
    plt.plot(history.epoch, history.history['loss'], '-o')
    plt.plot(history.epoch, history.history['val_loss'], '-o')
    #plt.ylim(0, 1)
    #plt.yticks(np.linspace(0, 1, 11))
    plt.xlim(0, len(history.epoch)-1)
    #plt.xticks(history.epoch)
    plt.ylabel('loss')
    plt.xlabel('epoch')
    return plt.gcf()

if __name__ == '__main__':
    X, y, _ = load_dataset(get_features)
    M, _ = train_model(X, y, test_size=0.2, final=True)
    print('saving model to res/model.pkl')
    save_model(M, 'res/model.pkl')
    