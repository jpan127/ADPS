import logging
import sys



def logger_create(name, level):
    """
    Factory for creating an initialized logger object
    @param name  : Name of the logger
    @param level : Level of logging messages by string
    @returns     : An initialized logger object
    """
    # TEXT_COLORS = {
    #     "BLUE" : "\033[94m",
    #     "NONE" : "\033[1;0m",
    #     "RED"  : "\033[1;31m",
    # }

    UNIT_TEST_LOGGING_LEVEL = 100

    # Callback for unit test logging
    def unit_test(self, msg, *args, **kwargs):
        if self.isEnabledFor(UNIT_TEST_LOGGING_LEVEL):
            # Colors the message
            self._log( #pylint: disable=protected-access
                UNIT_TEST_LOGGING_LEVEL,
                msg,
                args,
                **kwargs
            )

    # Register logging level + callback for unit_test
    logging.addLevelName(UNIT_TEST_LOGGING_LEVEL, "\033[94m{}\033[1;0m".format("UNIT_TEST"))
    logging.unit_test = unit_test
    logging.Logger.unit_test = unit_test

    # Converts the level string to the logging enumeration
    # This is so users do not need to import logging
    LOG_LEVEL_MAP = {
        "debug"     : logging.DEBUG,
        "info"      : logging.INFO,
        "warning"   : logging.WARNING,
        "error"     : logging.ERROR,
        "critical"  : logging.CRITICAL,
        "unit_test" : UNIT_TEST_LOGGING_LEVEL,
    }

    # Set some color
    logging.addLevelName(logging.WARNING , "\033[1;31m{}\033[1;0m".format(logging.getLevelName(logging.WARNING)))
    logging.addLevelName(logging.ERROR   , "\033[1;31m{}\033[1;0m".format(logging.getLevelName(logging.ERROR)))

    # Format string of logging
    FORMATTER = logging.Formatter('[%(levelname)s][%(module)s] %(message)s')

    if level not in LOG_LEVEL_MAP:
        raise ValueError("{} is not a valid level".format(level))

    # Create a handler for the logger to hook into
    handler = logging.StreamHandler(sys.stdout)
    handler.setFormatter(FORMATTER)
    handler.setLevel(LOG_LEVEL_MAP[level])

    # Create a logger object and return it
    logger = logging.getLogger(name)
    logger.addHandler(handler)
    logger.setLevel(LOG_LEVEL_MAP[level])

    return logger
