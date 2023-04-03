"""
File containing constant variables
"""

# Whether to run the training or evaluate from the best checkpoint
TRAIN = False
# Whether to use triplet loss for training
TRIPLET_LOSS = True
# This will be in the name of the folder in /out
EXPERIMENT_NAME = "best"

# Data info
IMAGES_PATH = "data/Images"
CSV_PATHS = ["data/train.csv", "data/valid.csv", "data/test.csv"]
X_COL_NAME = "filename"
Y_COL_NAME = "breed"
NUM_CLASSES = 120
IMAGE_SIZE = (299, 299, 3)

# Training info
BATCH_SIZE = 64
TRAIN_STEPS = 30000
INIT_LR = 0.1
LR_DECAY = 0.9
LR_DECAY_EPOCH_STEP = 3
WEIGHT_DECAY = 0.0001
MOMENTUM = 0.9
USE_NESTEROV = True
SEED = 42
