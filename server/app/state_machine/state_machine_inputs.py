from common.utils import logger_create



# All the state inputs
_INPUTS = [
    "left_ir_sensor",           # Left infrared sensor distance
    "right_ir_sensor",          # Right infrared sensor distance
    "bottom_ir_sensor",         # Bottom infrared sensor distance
    "vantage_angle",            # Vantage point angle in degrees
    "vpoint_timestamp",         # Vantage point angle received timestamp
    "found_house",              # Bool flag for found the house
    "found_path",               # Bool flag for found the path
    "ready_to_go_down_path",    # Bool flag for ready to go down path
    "ready_to_drop_package",    # Bool flag for ready to drop off package
    "dropped_off_package",      # Bool flag for already dropped off package
]

# Create a logger
_LOGGER = logger_create(__name__, "info")


class StateMachineInputs(object):
    """
    Object to encapsulate the various data that factor into the state machine transitions
    """

    def __init__(self, **kwargs):
        """
        Initializer
        For each state machine input, check if it was passed in, if it was, set it, else default to zero
        """
        for input_type in _INPUTS:
            self.__dict__[input_type] = kwargs.get(input_type, 0)

    async def update_input(self, input_type, value):
        """
        Update a state machine input with a new value
        @param input_type : The input by name
        @param value      : The new value
        """
        if input_type in self.__dict__:
            self.__dict__[input_type] = value
        else:
            _LOGGER.error("Input type invalid : %s", input_type)
