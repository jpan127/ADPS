# If using only CPU
import os
os.environ['CUDA_VISIBLE_DEVICES'] = '-1'

import keras
from keras.models import Sequential
from keras.layers import Dense, Activation, Dropout, Flatten
from keras.layers import MaxPooling2D, Conv2D
from keras import initializers
from keras.optimizers import RMSprop
from keras.preprocessing.image import ImageDataGenerator, array_to_img, img_to_array, load_img
import numpy as np
import sys
import argparse

#========================#
BATCH_SIZE      = 0
NUM_CLASSES     = 0
EPOCHS          = 0
STEPS_PER_EPOCH = 0
VALIDATION_STEPS = 0
HIDDEN_UNITS    = 0
LEARNING_RATE   = 0
#========================#

""" 
    @description            : Returns generators for augmented training data and non-augmented validation data
    @param dir_train        : Path to directory of training data
    @param dir_validation   : Path to directory of validation data
    @param class_mode       : Mode of data classification, 
                              @default : 'binary'
"""
def load_training_data(dir_train, dir_validation, class_mode='binary'):
    # Training data will be augmented
    train_datagen  = ImagedataGenerator(rescale=1./255, shear_range=0.2, zoom_range=0.2, horizontal_flip=True)
    # Testing data will not be augmented
    test_datagen   = ImageDataGenerator(rescale=1./255)
    # Load training data from specified directory
    train_gen      = train_datagen.flow_from_directory(dir_train, target_size=(150, 150), batch_size=BATCH_SIZE, class_mode=class_mode)
    # Load validation data from specified directory
    validation_gen = test_datagen.flow_from_directory(dir_validation, target_size=(150, 150), batch_size=BATCH_SIZE, class_mode=class_mode)
    
    return train_gen, validation_gen

#=================================================================================================#

""" Image needs to be a numpy array """
def augment_image(img, number, dir_augmented, name_prefix):
    # Randomly transform data
    # https://blog.keras.io/building-powerful-image-classification-models-using-very-little-data.html
    data_augmenter = ImageDataGenerator(    rotation_range=40,
                                            width_shift_range=0.2,
                                            height_shift_range=0.2,
                                            rescale=1./255,
                                            shear_range=0.2,
                                            zoom_range=0.2,
                                            horizontal_flip=True,
                                            fill_mode='nearest')
    # Generate 20 randomly augmented versions of x
    for i, batch in enumerate(data_augmenter.flow(img, batch_size=1, save_to_dir=dir_augmented, 
                                                    save_prefix=name_prefix, save_format='jpeg')):
        if i > number:
            break


# rmsprop = RMSprop(lr=LEARNING_RATE)

#=================================================================================================#

def create_model():
    # Create model
    model = Sequential()
    # Layer 1
    model.add(Conv2D(32, (3,3), input_shape=(3, 150, 150)))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=(2,2)))
    # Layer 2
    model.add(Conv2D(32, (3,3)))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=(2,2)))
    # Layer 3
    model.add(Conv2D(64, (3,3)))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=(2,2)))
    # Compile the model
    model.compile(loss='binary_crossentropy', optimizer='rmsprop', metrics=['accuracy'])

#=================================================================================================#

def train_model(dir_train, dir_validation, weights_name):
    # Create generators from datasets
    train_gen, validation_gen = load_training_data(dir_train, dir_validation)
    # Train the model
    model.fit_generator(train_gen,
                validation_data=validation_gen,
                steps_per_epoch=STEPS_PER_EPOCH // BATCH_SIZE,
                validation_steps=VALIDATION_STEPS // BATCH_SIZE,
                epochs=EPOCHS,
                verbose=1)
    model.save_weights(weights_name)

#=================================================================================================#

def main():
    if len(sys.argv) < 3:
        raise Exception("Missing arguments.  Example: python traffic.py <PATH_TO_TRAINING_DATA> <PATH_TO_VALIDATION_DATA>")

    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("--dir_training", help="path to training data")
    parser.add_argument("--dir_validation", help="path to validation data")
    args = parser.parse_args()
    print(args.dir_training)
    print(args.dir_validation)

    create_model()
    train_model(args.dir_training, args.dir_validation, "weights.h5")

if __name__ == '__main__':
    main()