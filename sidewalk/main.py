import os
import math
import numpy as np
import h5py
import random
from PIL import Image
import tensorflow as tf
from meta import Meta
import pprint as pp
import sys

# Assert python version is > 3 otherwise fail before crashing at some obscure code
assert sys.version_info >= (3,0), "\n  This script runs on Python 3+\n"

pp = pp.PrettyPrinter(indent=4)

""" A class for querying an object and its attributes from the SVHN digitStruct.mat file """
class DigitStructParser():

    """ Constructor"""
    def __init__(self, path):
        self.file = h5py.File(path, "r")
        # digitStruct.mat has two keys:
        # [0] '#refs#': i dont know
        # [1] 'digitStruct': the datasets    
        self.digit_struct = self.file['digitStruct']

        # 'digitStruct' has two keys:
        # [0] 'bbox': datasets
        # [1] 'name': name of the dataset
        self.name = self.file['digitStruct']['name']
        self.bbox = self.file['digitStruct']['bbox']

        # Since every image has the same attributes, looking at the attributes
        # of the first image is sufficient. Each image has 'MATLAB_fields'
        # and 'MATLAB_class'
        self.attributes = []
        for value in self.file[self.bbox[0][0]].attrs['MATLAB_fields']:
            self.attributes.append(value.tostring().decode('UTF-8'))

        self.data_set_size = 33402

    """ Used by the h5py.vistitems() method to iterate through the items """
    def __print_attr_iterator(self, name, obj):
        print(name)
        for key, value in obj.attrs.items():
            print("   ", key, value)

    """ 
        Returns the name of the element
        @param index: Number that corresponds to the element 
    """
    def get_name(self, index):
        # Looks up the value from the name dataset at index=index
        # Converts the numbers to char and returns as a string
        return ''.join([chr(x[0]) for x in self.file[self.name[index][0]].value])

    """ Returns a list of attributes of each element"""
    def get_attributes(self):
        return self.attributes

    """ 
        Returns a reference to an element's item
        @param index: Number that corresponds to the element 
    """
    def __get_item(self, index):
        return self.bbox[index].item()

    """ 
        Returns a dictionary of each value associated with each attribute of the element
        @param index: Number that corresponds to the element 
    """
    def get_attributes(self, index):
        values = {}
        item = self.__get_item(index)
        for attribute in self.attributes:
            reference = self.file[item][attribute]
            values[attribute] = [self.file[reference.value[i].item()].value[0][0] for i in range(len(reference))] if len(reference) > 1 else [reference.value[0][0]]
        return values

    """ 
        Returns a dictionary associating an element's file name to its dictionary of values
        @param index: Number that corresponds to the element 
    """
    def get_element(self, index):
        element = {}
        element[self.get_name(index)] = self.get_attributes(index)
        return element

    """ 
        Returns a value looked up by the element's attribute
        @param index     : Number that corresponds to the element 
        @param attribute : Enum that corresponds to one of the element's attributes
    """
    def get_single_attribute(self, index, attribute):
        if not attribute in self.attributes:
            return None
        else:
            return self.get_attributes(index)[attribute]

    def convert_to_tfrecords(self, data_path, start_index, end_index):
        all_data_paths = tf.gfile.Glob(os.path.join(data_path, "*.png"))
        # Loop through every path to every image
        for i, path in enumerate(all_data_paths[start_index:end_index]):
            # Calculate correct offset
            index = i + start_index
            # Get attributes
            attribute = self.get_attributes(index)
            # Get digits
            digits = [int(d) if d != 10 else 0 for d in attribute['label']]
            # Make sure each array is an integer by explictly casting (from numpy.float64)
            attrs_left, attrs_top, attrs_width, attrs_height = map(lambda x: [int(i) for i in x], 
                                                                  [attribute['left'], attribute['top'], attribute['width'], attribute['height']])
            # Create a bounding box
            min_left  = min(attrs_left)
            min_top   = min(attrs_top)
            max_right = max(map(lambda x, y: x + y, attrs_left, attrs_width))
            max_bot   = max(map(lambda x, y: x + y, attrs_top,  attrs_height))

            center_x = (min_left + max_right) / 2
            center_y = (min_top  + max_bot)   / 2
            max_side = max(max_right - min_left, max_bot - min_top)

            bbox_left   = center_x - max_side / 2
            bbox_top    = center_y - max_side / 2
            bbox_width  = max_side
            bbox_height = max_side

            # Determine cropping points
            crop_left   = int(round(bbox_left - 0.15 * bbox_width))
            crop_top    = int(round(bbox_top  - 0.15 * bbox_height))
            crop_width  = int(round(bbox_width  * 1.3))
            crop_height = int(round(bbox_height * 1.3))

            # Crop image
            image = Image.open(path)
            image = image.crop([crop_left, crop_top, crop_left + crop_width, crop_top + crop_height])
            image = image.resize([64, 64])
            image = np.array(image).tobytes()
            
            # Training features
            features = {
                'image'  : tf.train.Feature(bytes_list=tf.train.BytesList(value=[image])),
                'length' : tf.train.Feature(int64_list=tf.train.Int64List(value=[len(attribute['label'])])),
                'digits' : tf.train.Feature(int64_list=tf.train.Int64List(value=digits))
            }
            example = tf.train.Example(features=tf.train.Features(feature=features))

            # Do something here with the example
            break

    """ 
        Returns a dictionary of the number of images with 1 digit, 2 digits, ... 
        @note: Takes awhile so the results are below, should not change, but may be useful for other data sets
        {1: 5137, 2: 18130, 3: 8691, 4: 1434, 5: 9, 6: 1}
    """
    def get_distribution_of_label_lengths(self):
        histogram = {}
        for i in range(self.data_set_size):
            length = len(self.get_single_attribute(i, 'label'))
            if length in histogram:
                histogram[length] += 1
            else:
                histogram[length] = 1
        return histogram


def main():
    # Create h5py parser
    parser = DigitStructParser("data/train/digitStruct.mat")
    
    # Split data set up by ratio
    data_set_size = 33402
    training_data_set_percentage   = 0.60
    validation_data_set_percentage = (1 - training_data_set_percentage) / 2
    testing_data_set_percentage    = (1 - training_data_set_percentage) / 2

    # 0 - (0.6)(33402)
    training_data_set_indices   = ( 0, math.floor(data_set_size * training_data_set_percentage))
    # (0.6)(33402) - (0.8)(33402)
    validation_data_set_indices = ( math.floor(data_set_size * training_data_set_percentage) + 1, 
                                    math.floor(data_set_size * training_data_set_percentage + validation_data_set_percentage))
    # (0.8)(33402) - 33402
    validation_data_set_indices = ( math.floor(data_set_size * training_data_set_percentage + validation_data_set_percentage) + 1, 
                                    data_set_size)

    # Split data by indices

    # Convert into numpy arrays

    parser.convert_to_tfrecords("data/train", *training_data_set_indices)

""" Referencing this: https://github.com/potterhsu/SVHNClassifier/blob/master/convert_to_tfrecords.py """
if __name__ == '__main__':
    main()


"""
Thoughts:
    A little worried that these images only have 2-3 numbers when a house address in our area can be 5 digits
    We will need some way to understand these digits and read them from left to right
    An example is when the system reads each digit with a high enough confidence level, it can create bounding boxes for each of them,
    then determine which one is the leftmost, second from the leftmost, ... and this is how it will understand the order left to right.
"""