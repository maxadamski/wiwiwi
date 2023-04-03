from abc import ABC

import tensorflow as tf
from keras.layers import Dropout
from tensorflow.python.keras.applications.inception_resnet_v2 import InceptionResNetV2
from tensorflow.python.keras.layers import Dense


class IncResnetEmbedNorm(tf.keras.Model, ABC):
    def __init__(self, input_shape=(299, 299, 3)):
        super(IncResnetEmbedNorm, self).__init__()
        self.inc_res_v2 = InceptionResNetV2(
            include_top=False,
            pooling="avg",
            weights="imagenet",
            input_shape=input_shape,
        )
        for layer in self.inc_res_v2.layers:
            layer.trainable = False
        self.fc1 = Dense(512, activation="relu")
        self.dropout1 = Dropout(0.3)
        self.fc2 = Dense(256, activation=None)
        self.normalize = tf.keras.layers.Lambda(lambda x: tf.math.l2_normalize(x, axis=1))

    def call(self, inputs, **kwargs):
        x = self.inc_res_v2(inputs)
        x = self.dropout1(self.fc1(x))
        x = self.normalize(self.fc2(x))
        return x
