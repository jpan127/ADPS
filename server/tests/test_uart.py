import asyncio
import aiounittest
import unittest
from unittest.mock import Mock
import logging
import inspect

from common.utils import logger_create

from jetson_io.uart import LogPacket, process_uart



# Create a logger
_LOGGER = logger_create(__name__, "unit_test")
logging.disable(level=logging.DEBUG)
_LOG_TEST_FUNC = lambda : _LOGGER.unit_test(inspect.stack()[1].function)

class TestUart(aiounittest.AsyncTestCase):
    """
    Testing Uart
    """

    def test_log_packet_init(self):
        """
        """
        log_packet = LogPacket()
        self.assertEqual(log_packet.type    ,  0)
        self.assertEqual(log_packet.length  ,  0)
        self.assertEqual(log_packet.payload , "")

        kwargs = {
            "type"    : 123,
            "length"  : 456,
            "payload" : "awhefawefawuef9auwe9fawe9184012u408usdfua80sdfja80sdjf0",
        }

        log_packet = LogPacket(**kwargs)

        for arg in kwargs:
            self.assertEqual(log_packet.__dict__[arg], kwargs[arg])

        _LOG_TEST_FUNC()

    async def test_process_uart(self):
        TEST_QUEUE = asyncio.Queue()
        TEST_UART = Mock()
        TEST_PACKET = LogPacket()

        TEST_UART.read = Mock(
            return_value = "".join([
                "1",    # Type
                "2",    # Length
                "jp",   # Payload
            ])
        )

        await process_uart(
            TEST_QUEUE,
            TEST_UART,
            TEST_PACKET
        )

        #
        # self.assertEqual(TEST_PACKET.type, 1)
        # self.assertEqual(TEST_PACKET.length, 2)
        # self.assertEqual(TEST_PACKET.payload, "jp")