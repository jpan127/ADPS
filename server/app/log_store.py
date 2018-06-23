# Public libraries
import json



class LogStore(object):
    """
    Stores logging data in this container
    """

    def __init__(self, protocol_json_fname):
        """
        Initializer
        For each log type, create a class member variable
        @param protocol_json_fname : Name of JSON file that contains the log types
        """
        with open(protocol_json_fname, "r") as jfile:
            data = json.load(jfile)
            # Find the infrared logging enums
            for log_type in data["log_type"]:
                self.__dict__[log_type] = None

    async def update_log_data(self, log_type, value):
        """
        Updates a logging field with a new value
        @param log_type : The type of log to update
        @param value    : The new value to store
        """
        if log_type in self.__dict__:
            self.__dict__[log_type] = value
        else:
            raise ValueError("[ERROR] Log type invalid : {}".format(log_type))
