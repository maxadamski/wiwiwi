from abc import ABC

import tensorflow as tf
from keras.layers import Dropout
from tensorflow.python.keras.applications.inception_resnet_v2 import InceptionResNetV2
from tensorflow.python.keras.layers import Dense


class IncResnetClassify(tf.keras.Model, ABC):
    def __init__(self, num_classes, input_shape=(299, 299, 3)):
        super(IncResnetClassify, self).__init__()
        self.inc_res_v2 = InceptionResNetV2(
            include_top=False,
            pooling="avg",
            weights="imagenet",
            input_shape=input_shape,
        )
        for layer in self.inc_res_v2.layers:
            layer.trainable = False
        self.fc1 = Dense(512, activation="relu")
        self.dropout1 = Dropout(0.5)
        self.fc2 = Dense(256, activation="relu")
        self.dropout2 = Dropout(0.5)
        self.fc3 = Dense(num_classes, activation="softmax")

    def call(self, inputs, **kwargs):
        x = self.inc_res_v2(inputs)
        x = self.dropout1(self.fc1(x))
        x = self.dropout2(self.fc2(x))
        x = self.fc3(x)
        return x
