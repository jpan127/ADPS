import unittest
import aiounittest
import json
import logging
import random
import inspect
import os
import asyncio

from common.utils import logger_create

from app.log_store import LogStore



# Create a logger
_LOGGER = logger_create(__name__, "unit_test")
logging.disable(level=logging.DEBUG)
_LOG_TEST_FUNC = lambda : _LOGGER.unit_test(inspect.stack()[1].function)

class TestLogStore(aiounittest.AsyncTestCase):
    """
    Testing LogStore
    """

    TEST_JSON_FILE = "test.json"

    TEST_JSON_CONTENTS = {
        "type" : {
            "info"   : 0,
            "error"  : 1,
            "status" : 2,
            "log"    : 3,
        },
        "log_type" : {
            "client"          : 0,
            "server"          : 1,
            "packet"          : 2,
            "motor"           : 3,
            "wifi"            : 4,
            "wmark"           : 5,
            "infrared_left"   : 6,
            "infrared_right"  : 7,
            "infrared_bottom" : 8,
        },
        "opcode" : {
            "move_forward"      : 0,
            "move_backward"     : 1,
            "move_left"         : 2,
            "move_right"        : 3,
            "incr_forward"      : 4,
            "incr_backward"     : 5,
            "incr_left"         : 6,
            "incr_right"        : 7,
            "decr_forward"      : 8,
            "decr_backward"     : 9,
            "decr_left"         : 10,
            "decr_right"        : 11,
            "servo_duty"        : 12,
            "manual_mode"       : 13,
            "stop"              : 14,
            "deliver"           : 15,
            "pivot_left"        : 16,
            "pivot_right"       : 17,
            "change_state"      : 18,
            "execute_self_test" : 19,
            "set_suspend"       : 20,
        }
    }

    def setUp(self):
        """
        Set up function
        Opens a JSON file and always writes the exact same contents
        """
        with open(TestLogStore.TEST_JSON_FILE, "w") as jfile:
            jfile.write(json.dumps(TestLogStore.TEST_JSON_CONTENTS))

    def tearDown(self):
        """
        Removes the test file
        """
        os.remove(TestLogStore.TEST_JSON_FILE)

    async def test_update_log_data(self):
        """
        """
        random.seed(127)

        # Create object under test first
        log_store = LogStore(TestLogStore.TEST_JSON_FILE)

        # Make sure every log_type is created and initialied as None
        for log_type in TestLogStore.TEST_JSON_CONTENTS["log_type"]:
            self.assertEqual(log_store.__dict__[log_type], None)

        # Update the data with random values
        for log_type in TestLogStore.TEST_JSON_CONTENTS["log_type"]:
            random_value = random.randint(0, 9999999999999)
            await log_store.update_log_data(log_type, random_value)
            self.assertEqual(log_store.__dict__[log_type], random_value)

        # Update with invalid values
        with self.assertRaises(ValueError):
            await log_store.update_log_data("INVALID_TYPE", "INVALID_VALUE")

        _LOG_TEST_FUNC()
