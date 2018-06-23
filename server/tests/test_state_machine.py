import unittest
import asyncio
import aiounittest
import inspect
import logging

from common.utils import logger_create

from app.state_machine.state_machine import StateMachine, SystemState
from app.state_machine.state_machine_inputs import StateMachineInputs



# Create a logger
_LOGGER = logger_create(__name__, "unit_test")
logging.disable(level=logging.DEBUG)
_LOG_TEST_FUNC = lambda : _LOGGER.unit_test(inspect.stack()[1].function)

class TestStateMachine(aiounittest.AsyncTestCase):
    """
    Tests the class StateMachine
    """

    async def test_navigating_sidewalk(self):
        state_machine_inputs = StateMachineInputs()
        state_machine = StateMachine(state_machine_inputs)
        self.assertEqual(state_machine.current_state, SystemState.NAVIGATING_SIDEWALK)
        _LOGGER.info(inspect.stack()[0].function)

    async def test_looking_for_path(self):
        state_machine_inputs = StateMachineInputs()
        state_machine = StateMachine(state_machine_inputs)
        # Stays in NAVIGATING_SIDEWALK
        state_machine_inputs.found_house = False
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.NAVIGATING_SIDEWALK)
        # Transition to LOOKING_FOR_PATH
        state_machine_inputs.found_house = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.LOOKING_FOR_PATH)
        _LOG_TEST_FUNC()

    async def test_found_path_to_found(self):
        state_machine_inputs = StateMachineInputs()
        state_machine = StateMachine(state_machine_inputs)
        # Transition to LOOKING_FOR_PATH
        state_machine_inputs.found_house = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.LOOKING_FOR_PATH)
        # Stay in LOOKING_FOR_PATH
        state_machine_inputs.found_path = False
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.LOOKING_FOR_PATH)
        # Transition to FOUND_PATH_TO_HOUSE
        state_machine_inputs.found_path = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.FOUND_PATH_TO_HOUSE)
        _LOG_TEST_FUNC()

    async def test_navigation_path(self):
        state_machine_inputs = StateMachineInputs()
        state_machine = StateMachine(state_machine_inputs)
        # Transition to LOOKING_FOR_PATH
        state_machine_inputs.found_house = True
        await state_machine.run()
        # Transition to FOUND_PATH_TO_HOUSE
        state_machine_inputs.found_path = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.FOUND_PATH_TO_HOUSE)
        # Stay in FOUND_PATH_TO_HOUSE
        state_machine_inputs.ready_to_go_down_path = False
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.FOUND_PATH_TO_HOUSE)
        # Transition to NAVIGATING_PATH
        state_machine_inputs.ready_to_go_down_path = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.NAVIGATING_PATH)
        _LOG_TEST_FUNC()

    async def test_reached_door(self):
        state_machine_inputs = StateMachineInputs()
        state_machine = StateMachine(state_machine_inputs)
        # Transition to LOOKING_FOR_PATH
        state_machine_inputs.found_house = True
        await state_machine.run()
        # Transition to FOUND_PATH_TO_HOUSE
        state_machine_inputs.found_path = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.FOUND_PATH_TO_HOUSE)
        # Transition to NAVIGATING_PATH
        state_machine_inputs.ready_to_go_down_path = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.NAVIGATING_PATH)
        # Stay in NAVIGATING_PATH
        state_machine_inputs.left_ir_sensor   = 50
        state_machine_inputs.right_ir_sensor  = 50
        state_machine_inputs.bottom_ir_sensor = 50
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.NAVIGATING_PATH)
        # Transition to REACHED_DOOR
        state_machine_inputs.left_ir_sensor   = 49
        state_machine_inputs.right_ir_sensor  = 49
        state_machine_inputs.bottom_ir_sensor = 49
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.REACHED_DOOR)
        _LOG_TEST_FUNC()

    async def test_deliver_package(self):
        state_machine_inputs = StateMachineInputs()
        state_machine = StateMachine(state_machine_inputs)
        # Transition to LOOKING_FOR_PATH
        state_machine_inputs.found_house = True
        await state_machine.run()
        # Transition to FOUND_PATH_TO_HOUSE
        state_machine_inputs.found_path = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.FOUND_PATH_TO_HOUSE)
        # Transition to NAVIGATING_PATH
        state_machine_inputs.ready_to_go_down_path = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.NAVIGATING_PATH)
        # Transition to REACHED_DOOR
        state_machine_inputs.left_ir_sensor   = 49
        state_machine_inputs.right_ir_sensor  = 49
        state_machine_inputs.bottom_ir_sensor = 49
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.REACHED_DOOR)
        # Stay in REACHED_DOOR
        state_machine_inputs.ready_to_drop_package = False
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.REACHED_DOOR)
        # Transition to DELIVER_PACKAGE
        state_machine_inputs.ready_to_drop_package = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.DELIVER_PACKAGE)
        _LOG_TEST_FUNC()

    async def test_reset_resquence(self):
        state_machine_inputs = StateMachineInputs()
        state_machine = StateMachine(state_machine_inputs)
        # Transition to LOOKING_FOR_PATH
        state_machine_inputs.found_house = True
        await state_machine.run()
        # Transition to FOUND_PATH_TO_HOUSE
        state_machine_inputs.found_path = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.FOUND_PATH_TO_HOUSE)
        # Transition to NAVIGATING_PATH
        state_machine_inputs.ready_to_go_down_path = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.NAVIGATING_PATH)
        # Transition to REACHED_DOOR
        state_machine_inputs.left_ir_sensor   = 49
        state_machine_inputs.right_ir_sensor  = 49
        state_machine_inputs.bottom_ir_sensor = 49
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.REACHED_DOOR)
        # Transition to DELIVER_PACKAGE
        state_machine_inputs.ready_to_drop_package = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.DELIVER_PACKAGE)
        # Stay in DELIVER_PACKAGE
        state_machine_inputs.dropped_off_package = False
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.DELIVER_PACKAGE)
        # Transition to NAVIGATING_SIDEWALK
        state_machine_inputs.dropped_off_package = True
        await state_machine.run()
        self.assertEqual(state_machine.current_state, SystemState.NAVIGATING_SIDEWALK)
        _LOG_TEST_FUNC()

