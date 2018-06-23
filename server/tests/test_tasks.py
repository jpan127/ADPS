import asyncio
import inspect
import logging
import os
import json
import asynctest
from asynctest import patch

from common.utils import logger_create

from tasks import tasks
from app.state_machine.state_machine import StateMachine, SystemState
from app.state_machine.state_machine_inputs import StateMachineInputs
from app.log_store import LogStore
from jetson_io.uart import LogPacket



# Create a logger
_LOGGER = logger_create(__name__, "unit_test")
logging.disable(level=logging.DEBUG)
_LOG_TEST_FUNC = lambda : _LOGGER.unit_test(inspect.stack()[1].function)

class TestTask_ConsumeLoggingPackets(asynctest.TestCase):
    """
    Tests the class StateMachine
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
        os.environ["ENVIRONMENT"] = "Unit Test"
        with open(TestTask_ConsumeLoggingPackets.TEST_JSON_FILE, "w") as jfile:
            jfile.write(json.dumps(TestTask_ConsumeLoggingPackets.TEST_JSON_CONTENTS))

    def tearDown(self):
        """
        Removes the test file
        """
        os.remove(TestTask_ConsumeLoggingPackets.TEST_JSON_FILE)

    async def test_task_consume_logging_packets(self):
        """
        Tests an infrared_log_type packet in the queue
        """
        TEST_QUEUE     = asyncio.Queue()
        TEST_SM_INPUTS = StateMachineInputs()
        TEST_LOG_STORE = LogStore(TestTask_ConsumeLoggingPackets.TEST_JSON_FILE)
        TEST_PACKET    = LogPacket(type=6, length=1, payload="x")

        await TEST_QUEUE.put(TEST_PACKET)

        with patch("tasks.tasks.state_machine_inputs.StateMachineInputs.update_input") as patcher:
            await tasks.task_consume_logging_packets(
                TEST_QUEUE,
                TEST_SM_INPUTS,
                TEST_LOG_STORE,
                TestTask_ConsumeLoggingPackets.TEST_JSON_FILE
            )
            patcher.assert_called_once_with(6, "x")

    async def test_task_state_machine(self):
        """
        This task does not really need to be tested, all it does is run the StateMachine and sleep
        """

        TEST_INPUTS = StateMachineInputs()
        await tasks.task_state_machine(TEST_INPUTS)

    async def test_task_open_browser(self):
        """
        Test open browser and dont open browser
        """
        with patch("tasks.tasks.webbrowser.open_new_tab") as patcher:
            await tasks.task_open_browser(True)
            mock_method = patcher.return_value
            patcher.assert_called_once_with(url="http://127.0.0.1:8080")

        with patch("tasks.tasks.webbrowser.open_new_tab") as patcher:
            await tasks.task_open_browser(False)
            patcher.assert_not_called()
