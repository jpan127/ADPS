import unittest
import asyncio
import aiounittest
import inspect
import logging
import random

from common.utils import logger_create

from app.state_machine.state_machine import StateMachine, SystemState
from app.state_machine.state_machine_inputs import StateMachineInputs



# Create a logger
_LOGGER = logger_create(__name__, "unit_test")
logging.disable(level=logging.ERROR)
_LOG_TEST_FUNC = lambda : _LOGGER.unit_test(inspect.stack()[1].function)

class TestStateMachine(aiounittest.AsyncTestCase):
    """
    Tests the class StateMachine
    """

    # All the state inputs
    INPUTS = [
        "left_ir_sensor",
        "right_ir_sensor",
        "bottom_ir_sensor",
        "vantage_angle",
        "vpoint_timestamp",
        "found_house",
        "found_path",
        "ready_to_go_down_path",
        "ready_to_drop_package",
        "dropped_off_package",
    ]

    async def test_init(self):
        """
        Test initializing the object's member variables
        """
        # Test zero initial values
        obj_under_test = StateMachineInputs()
        for input in TestStateMachine.INPUTS:
            self.assertEqual(obj_under_test.__dict__[input], 0)

        # Test all initial values
        TEST_VALUE = 123
        kwargs = { input : TEST_VALUE for input in TestStateMachine.INPUTS }
        obj_under_test = StateMachineInputs(**kwargs)
        for input in TestStateMachine.INPUTS:
            self.assertEqual(obj_under_test.__dict__[input], TEST_VALUE)

        # Test some initial values
        TEST_VALUE = 8687687
        random_samples = random.sample(TestStateMachine.INPUTS, 5)
        kwargs = { sample : TEST_VALUE for sample in random_samples }
        obj_under_test = StateMachineInputs(**kwargs)
        for sample in random_samples:
            self.assertEqual(obj_under_test.__dict__[sample], TEST_VALUE)

        _LOG_TEST_FUNC()

    async def test_update_input(self):
        """
        Test updating inputs to new values
        """
        # Test zero initial values
        obj_under_test = StateMachineInputs()
        for i, input in enumerate(TestStateMachine.INPUTS):
            self.assertEqual(obj_under_test.__dict__[input], 0)
            await obj_under_test.update_input(input, i)
            self.assertEqual(obj_under_test.__dict__[input], i)

        # Test all initial values
        TEST_VALUE = 123
        kwargs = { input : TEST_VALUE for input in TestStateMachine.INPUTS }
        obj_under_test = StateMachineInputs(**kwargs)
        for i, input in enumerate(TestStateMachine.INPUTS):
            self.assertEqual(obj_under_test.__dict__[input], TEST_VALUE)
            await obj_under_test.update_input(input, i)
            self.assertEqual(obj_under_test.__dict__[input], i)

        # Test some initial values
        TEST_VALUE = 8687687
        random_samples = random.sample(TestStateMachine.INPUTS, 5)
        kwargs = { sample : TEST_VALUE for sample in random_samples }
        obj_under_test = StateMachineInputs(**kwargs)
        for i, sample in enumerate(random_samples):
            self.assertEqual(obj_under_test.__dict__[sample], TEST_VALUE)
            await obj_under_test.update_input(input, i)
            self.assertEqual(obj_under_test.__dict__[input], i)

        # Test invalid inputs
        await obj_under_test.update_input("INVALID_INPUT", "INVALID")
        _LOG_TEST_FUNC()

