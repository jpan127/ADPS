# Public libraries
import json
import asyncio
import inspect
import os
import webbrowser

# Project libraries
from app.state_machine.state_machine import StateMachine
from app.state_machine import state_machine_inputs #pylint: disable=unused-import
from common.utils import logger_create



_LOGGER = logger_create(__name__, "debug")

async def task_consume_logging_packets(packet_queue, sm_inputs, log_store, protocol_json_fname):
    """
    Coroutine
    @param packet_queue        : Queue of packets parsed to dequeue from
    @param sm_inputs           : State machine inputs to update
    @param log_store           : Logging data storage structure
    @param protocol_json_fname : Name of protocol JSON file
    """

    INFRARED_LOG_TYPES = [
        "infrared_left",
        "infrared_right",
        "infrared_bottom",
    ]

    # Map the enumeration (value) to the name (key)
    infrared_log_enums = { }

    log_enum_map = { }

    # Read the JSON file
    with open(protocol_json_fname, "r") as jfile:
        data = json.load(jfile)
        log_types = data["log_type"]
        # Find the infrared logging enums
        for log_type, enum in log_types.items():
            log_enum_map[enum] = log_type
            if log_type in INFRARED_LOG_TYPES:
                infrared_log_enums[enum] = log_type

    _LOGGER.info("%s starting...", inspect.stack()[0].function)

    # Function that encapsulates what the task does
    async def consumer_logging_packets():
        # Dequeue packet, blocking
        packet = await packet_queue.get()

        # Check if packet has valid type
        if packet.type not in log_types.values():
            _LOGGER.error("Received unknown type: %i", packet.type)
        else:
            _LOGGER.info(packet.payload)
            # Infrared logs should update the state machine
            if packet.type in infrared_log_enums:
                await sm_inputs.update_input(
                    packet.type,
                    packet.payload
                )
            # Always update the logging store
            await log_store.update_log_data(log_enum_map[packet.type], packet.payload)

    # In the unit test environment, execute the operation once, otherwise run an infinite loop
    if "ENVIRONMENT" in os.environ and os.environ["ENVIRONMENT"] == "Unit Test":
        await consumer_logging_packets()
    else: # pragma: no cover
        # Main loop
        while True:
            await consumer_logging_packets()


async def task_state_machine(sm_inputs):
    """
    Coroutine
    Handles the state machine transitions
    @param sm_inputs : Reference to a StateMachineInputs object
    @task            : This is an infinite loop task
    """

    # How often the task should wake up and check for potential state transitions
    STATE_TRANSITION_FREQUENCY_HZ = 1

    # How often in seconds
    TIME_BETWEEN_STATE_TRANSITIONS_SEC = 1 / STATE_TRANSITION_FREQUENCY_HZ

    # State machine object
    state_machine = StateMachine(sm_inputs)

    _LOGGER.info("%s starting...", inspect.stack()[0].function)

    # Function that encapsulates what the task does
    async def run_state_machine():
        # Check for state transitions and transition if necessary
        await state_machine.run()
        # Sleep between state transition checks
        await asyncio.sleep(TIME_BETWEEN_STATE_TRANSITIONS_SEC)

    # In the unit test environment, execute the operation once, otherwise run an infinite loop
    if "ENVIRONMENT" in os.environ and os.environ["ENVIRONMENT"] == "Unit Test":
        await run_state_machine()
    else: # pragma: no cover
        # Main loop
        while True:
            await run_state_machine()


async def task_open_browser(open_browser):
    """
    Coroutine
    Opens a tab in the current device's default browser to the URL in which the server is being served
    @param open_browser <bool> : True to open, false to skip
    """
    if open_browser:
        webbrowser.open_new_tab(url="http://127.0.0.1:8080")
        _LOGGER.info("starting browser")
