# Public libraries
from enum import Enum
from common.utils import logger_create



_LOGGER = logger_create(__name__, "debug")


class SystemState(Enum):
    """
    Enumerates the different states of the state machine
    """
    NAVIGATING_SIDEWALK  = 0 # Still on the sidewalk
    LOOKING_FOR_PATH     = 1 # Found house but looking for path to the door
    FOUND_PATH_TO_HOUSE  = 2 # Found the path to door, need to pivot and start on it
    NAVIGATING_PATH      = 3 # Going down the path to the door
    REACHED_DOOR         = 4 # Got to the end of the path
    DELIVER_PACKAGE      = 5 # Activate delivery actuators


"""
@purpose:
Each of these functions are a callback for the state machine
Each function is given the current state and all the potential inputs
"""

async def _navigating_sidewalk_state_callback(current_state, state_inputs):
    _LOGGER.debug("navigating_sidewalk_state_callback")
    if state_inputs.found_house:
        return SystemState.LOOKING_FOR_PATH
    else:
        return current_state


async def _looking_for_path_state_callback(current_state, state_inputs):
    _LOGGER.debug("looking_for_path_state_callback")
    if state_inputs.found_path:
        return SystemState.FOUND_PATH_TO_HOUSE
    else:
        return current_state


async def _found_path_to_house_state_callback(current_state, state_inputs):
    _LOGGER.debug("found_path_to_house_state_callback")
    if state_inputs.ready_to_go_down_path:
        return SystemState.NAVIGATING_PATH
    else:
        return current_state


async def _navigating_path_state_callback(current_state, state_inputs):
    _LOGGER.debug("navigating_path_state_callback")
    sensor_values = [
        state_inputs.left_ir_sensor,
        state_inputs.right_ir_sensor,
        state_inputs.bottom_ir_sensor,
    ]
    if any(sensor_value < 50 for sensor_value in sensor_values):
        return SystemState.REACHED_DOOR
    else:
        return current_state


async def _reached_door_state_callback(current_state, state_inputs):
    _LOGGER.debug("reached_door_state_callback")
    if state_inputs.ready_to_drop_package:
        return SystemState.DELIVER_PACKAGE
    else:
        return current_state


async def _deliver_package_state_callback(current_state, state_inputs):
    _LOGGER.debug("deliver_package_state_callback")
    if state_inputs.dropped_off_package:
        return SystemState.NAVIGATING_SIDEWALK
    else:
        return current_state


class StateMachine(object):
    """
    A state machine to transition between the system's main states
    """

    # Contains the designated callbacks for each state
    STATE_TRANSITION_TABLE = {
        SystemState.NAVIGATING_SIDEWALK : _navigating_sidewalk_state_callback ,
        SystemState.LOOKING_FOR_PATH    : _looking_for_path_state_callback    ,
        SystemState.FOUND_PATH_TO_HOUSE : _found_path_to_house_state_callback ,
        SystemState.NAVIGATING_PATH     : _navigating_path_state_callback     ,
        SystemState.REACHED_DOOR        : _reached_door_state_callback        ,
        SystemState.DELIVER_PACKAGE     : _deliver_package_state_callback     ,
    }

    def __init__(self, inputs):
        """
        Initializer
        @param inputs <StateMachineInputs> : Reference to a StateMachineInputs object
        """
        self.state_inputs  = inputs
        self.current_state = SystemState.NAVIGATING_SIDEWALK

    async def run(self):
        """
        Look up the callback for the current state and execute it
        """
        # Find the correct callback to handle the current state
        callback = StateMachine.STATE_TRANSITION_TABLE[self.current_state]
        # Execute the callback, the current state may or my not change
        self.current_state = await callback(self.current_state, self.state_inputs)
