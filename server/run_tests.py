"""
Runs all the tests in the test directory
"""
import unittest
import os



# Configuration
TEST_DIR  = "tests"
VERBOSITY = 0

# Define this function so unittest knows how to find tests
def load_tests(loader, tests, pattern):
    return loader.discover(".")

if __name__ == '__main__':
    os.chdir(TEST_DIR)
    unittest.main(verbosity=VERBOSITY)
