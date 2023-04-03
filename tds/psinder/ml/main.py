import os
import io
from loguru import logger
from datetime import datetime

import numpy as np
import tensorflow as tf
import pandas as pd
from keras.callbacks import ModelCheckpoint, TensorBoard
from keras_preprocessing.image import ImageDataGenerator
from tensorflow.python.keras.applications.inception_resnet_v2 import preprocess_input
from tensorflow_addons.optimizers import SGDW
from tensorflow_addons.losses import TripletSemiHardLoss

from config import CSV_PATHS, IMAGES_PATH, Y_COL_NAME, X_COL_NAME, BATCH_SIZE, IMAGE_SIZE, SEED, NUM_CLASSES, \
    TRAIN_STEPS, INIT_LR, LR_DECAY_EPOCH_STEP, LR_DECAY, WEIGHT_DECAY, MOMENTUM, USE_NESTEROV, TRAIN, TRIPLET_LOSS, \
    EXPERIMENT_NAME
from models.inc_resnet_classify import IncResnetClassify
from models.inc_resnet_embed_norm import IncResnetEmbedNorm


def get_data_loaders(train_csv_path, valid_csv_path, test_csv_path,
                     data_path, x_col_name, y_col_name, batch_size,
                     image_size, seed, class_mode,
                     preprocess_fun, rotation=30, horizontal_flip=True):
    datagen = ImageDataGenerator(
        preprocessing_function=preprocess_fun, rotation_range=rotation, horizontal_flip=horizontal_flip
    )
    train_generator = datagen.flow_from_dataframe(
        dataframe=pd.read_csv(train_csv_path),
        directory=data_path,
        x_col=x_col_name,
        y_col=y_col_name,
        batch_size=batch_size,
        seed=seed,
        shuffle=True,
        class_mode=class_mode,
        target_size=image_size,
    )

    test_datagen = ImageDataGenerator(preprocessing_function=preprocess_input)
    valid_generator = test_datagen.flow_from_dataframe(
        dataframe=pd.read_csv(valid_csv_path),
        directory=data_path,
        x_col=x_col_name,
        y_col=y_col_name,
        batch_size=batch_size,
        seed=seed,
        shuffle=True,
        class_mode=class_mode,
        target_size=image_size,
    )

    test_generator = test_datagen.flow_from_dataframe(
        dataframe=pd.read_csv(test_csv_path),
        directory=data_path,
        x_col=x_col_name,
        y_col=y_col_name,
        batch_size=batch_size,
        seed=seed,
        shuffle=False,
        class_mode=class_mode,
        target_size=image_size,
    )

    return train_generator, valid_generator, test_generator


def prepare_and_train(train_loader, valid_loader, logs_loc, triplet_loss, image_size, train_steps, num_classes,
                      init_lr, lr_decay, lr_decay_epoch_step, weight_decay, momentum, use_nesterov):
    step_size_train = train_loader.n // train_loader.batch_size
    step_size_valid = valid_loader.n // valid_loader.batch_size

    lr_schedule = tf.keras.optimizers.schedules.ExponentialDecay(
        initial_learning_rate=init_lr, decay_steps=step_size_train * lr_decay_epoch_step, decay_rate=lr_decay
    )
    opt = SGDW(
        weight_decay=weight_decay, momentum=momentum, learning_rate=lr_schedule, nesterov=use_nesterov
    )

    if triplet_loss:
        model = IncResnetEmbedNorm(input_shape=image_size)
        model.compile(optimizer=opt, loss=TripletSemiHardLoss())
        monitor = "val_loss"
        checkpoint_mode = 'min'
    else:
        model = IncResnetClassify(num_classes=num_classes, input_shape=image_size)
        model.compile(optimizer=opt, loss="categorical_crossentropy", metrics=["accuracy"])
        monitor = "val_accuracy"
        checkpoint_mode = 'max'

    filepath = f"out/{logs_loc}/checkpoints/weights-improvement-{{epoch:02d}}-{{{monitor}:.4f}}"
    checkpoint = ModelCheckpoint(filepath, monitor=monitor, verbose=1, save_best_only=True, mode=checkpoint_mode)
    tensorboard = TensorBoard(
        log_dir=f'out/{logs_loc}/logs',
        histogram_freq=1,
        write_images=True
    )
    callbacks_list = [checkpoint, tensorboard]
    history = model.fit(
        train_loader,
        steps_per_epoch=step_size_train,
        validation_data=valid_loader,
        validation_steps=step_size_valid,
        epochs=train_steps // step_size_train,
        callbacks=callbacks_list
    )
    np.save(f'out/{logs_loc}/my_history.npy', history.history)
    return model


if __name__ == "__main__":
    physical_devices = tf.config.experimental.list_physical_devices("GPU")
    assert len(physical_devices) > 0, "Not enough GPU hardware devices available"
    tf.config.experimental.set_memory_growth(physical_devices[0], True)
    logger.debug(f"Passed GPU sanity check, devices:{physical_devices}")

    logger.debug(
        f"Running "
        f"{'training and evaluation' if TRAIN else 'evaluation'} "
        f"{'of embedding with triplet loss' if TRIPLET_LOSS else 'of classification'} "
        f"using InceptionResnetv2 backbone")

    log_dir = 'triplet' if TRIPLET_LOSS else 'classification'
    log_dir = f"{log_dir}_{EXPERIMENT_NAME}"
    class_mode = 'sparse' if TRIPLET_LOSS else 'categorical'

    logger.debug(f"Loading data")
    train_loader, valid_loader, test_loader = get_data_loaders(CSV_PATHS[0], CSV_PATHS[1], CSV_PATHS[2],
                                                               IMAGES_PATH, X_COL_NAME, Y_COL_NAME, BATCH_SIZE,
                                                               IMAGE_SIZE[:2], SEED, class_mode, preprocess_input,
                                                               rotation=30, horizontal_flip=True)

    if TRAIN:
        logger.debug(f"Starting training")
        model = prepare_and_train(train_loader, valid_loader, log_dir, TRIPLET_LOSS, IMAGE_SIZE, TRAIN_STEPS, NUM_CLASSES,
                                  INIT_LR, LR_DECAY, LR_DECAY_EPOCH_STEP, WEIGHT_DECAY, MOMENTUM, USE_NESTEROV)
    else:
        best = max(os.listdir(f"out/{log_dir}/checkpoints/"))
        logger.debug(f"Loading best model from checkpoint: {best}")
        model = tf.keras.models.load_model(f'out/{log_dir}/checkpoints/{best}')

    logger.debug(f"Starting evaluation")
    step_size_test = test_loader.n // test_loader.batch_size
    model.evaluate(x=test_loader, steps=step_size_test)

    if TRIPLET_LOSS:
        logger.debug(f"Starting prediction and saving data for feature space visualization")
        results = model.predict(test_loader)

        # Save test embeddings for visualization in projector
        np.savetxt(f"out/{log_dir}/vecs.tsv", results, delimiter='\t')

        out_m = io.open(f'out/{log_dir}/meta.tsv', 'w', encoding='utf-8')
        for i, (images, labels) in enumerate(test_loader):
            [out_m.write(str(int(x)) + "\n") for x in labels]
            if i >= step_size_test:  # manually detect the end of the epoch
                break
        out_m.close()
