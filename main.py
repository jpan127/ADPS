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
    def get_value(self, index):
        values = {}
        item = self.__get_item(index)
        for attribute in self.attributes:
            reference = self.file[item][attribute]
            values[attribute] = [self.file[reference.value[i].item()].value[0][0] for i in range(len(reference))]
        return values

    """ 
        Returns a dictionary associating an element's file name to its dictionary of values
        @param index: Number that corresponds to the element 
    """
    def get_element(self, index):
        element = {}
        element[self.get_name(index)] = self.get_value(index)
        return element

    """ 
        Returns a value looked up by the element's attribute
        @param index     : Number that corresponds to the element 
        @param attribute : Enum that corresponds to one of the element's attributes
    """
    def get_attribute_of_element(self, index, attribute):
        if not attribute in self.attributes:
            return None
        else:
            return self.get_value(index)[attribute]


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

    # Split data by indicies

    # Convert into numpy arrays

if __name__ == '__main__':
    main()