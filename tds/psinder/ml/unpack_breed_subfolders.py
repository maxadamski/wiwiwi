import shutil
import os

# Script to unpack breed subfolders in original dataset
parent = "./data/Images"
files_list = [os.path.join(parent, directory) for directory in os.listdir(parent)]
files_list = list(filter(os.path.isdir, files_list))
for subfolder in files_list:
    for file in os.listdir(subfolder):
        shutil.move(os.path.join(subfolder, file), parent)
    os.rmdir(subfolder)
