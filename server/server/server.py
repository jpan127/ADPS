# Public libraries
import asyncio
import sys
import json
import serial
import aiohttp
from aiohttp import web

# Project libraries
from tasks import tasks
from app.state_machine.state_machine_inputs import StateMachineInputs
from app.log_store import LogStore
from jetson_io.uart.microcontroller.uart import process_uart, LogPacket



routes = web.RouteTableDef()

@routes.get("/")
async def index(request): #pylint: disable=unused-argument
    with open("views/index.html") as page:
        content = page.read()
        return web.Response(text=content, content_type="text/html")


@routes.get("/controls")
async def controls(request): #pylint: disable=unused-argument
    with open("views/controls.html") as page:
        content = page.read()
        return web.Response(text=content, content_type="text/html")


@routes.get("/camera")
async def camera(request): #pylint: disable=unused-argument
    with open("views/camera.html") as page:
        content = page.read()
        return web.Response(text=content, content_type="text/html")


@routes.post("/controls/movement/{ACTION}")
async def movement(request):
    action = request.match_info.get("ACTION")
    print(action)
    return web.Response(text="")


@routes.get("/exit")
async def server_exit(request): #pylint: disable=unused-argument
    print("Exiting...")
    sys.exit()


class ApdsServer(object):
    """
    Encapsulates the initialization of an aiohttp server
    """

    def __init__(self, run_server, run_app, open_browser):
        """
        Initializer
        @param run_server   : Open browser on application start
        @param run_app      : Run the web server
        @param open_browser : Run the background tasks
        """
        self.open_browser = open_browser

        if run_server:
            # Main asynchronous event loop
            event_loop = asyncio.get_event_loop()
            self.app = event_loop.run_until_complete(self._create_app())
            if run_app:
                self.app.on_startup.append(self._register_server_background_tasks_on_startup)
                self.app.on_cleanup.append(self._await_server_background_tasks_on_cleanup)
                self.app.on_shutdown.append(self._await_server_background_tasks_on_cleanup)
            aiohttp.web.run_app(self.app)

    async def _create_app(self):
        """
        Initializes the application and returns it
        """
        # Create an aiohttp application
        app = web.Application()
        # Link the routes decorator to the routing table
        app.router.add_routes(routes)
        # Add path to images as "current path"
        app.router.add_static("/", path="static")
        return app

    async def _await_server_background_tasks_on_cleanup(self, app):
        """
        Cleanup handler for active tasks
        """
        print("Cleaning up open tasks...")
        for task in app["background_tasks"]:
            task.cancel()
            await task

    async def _register_server_background_tasks_on_startup(self, app):
        """
        Startup handler for registering background tasks
        """
        CONFIG_JSON_FILE = "config.json"

        # Parse configurations
        with open(CONFIG_JSON_FILE, "r") as config_file:
            configuration = json.load(config_file)
            protocol_json_file = configuration["protocol_json_file"]["value"]

        # Shared task variables
        state_machine_inputs = StateMachineInputs()         # Bundled state machine inputs object
        log_store            = LogStore(protocol_json_file) # Stores logging data
        packet_queue         = asyncio.Queue(100)           # Logging packet queue

        # Tasks that run in the background of the web server
        app["background_tasks"] = [
            app.loop.create_task(
                tasks.task_state_machine(
                    state_machine_inputs
                )
            ),
            app.loop.create_task(
                tasks.task_consume_logging_packets(
                    packet_queue,
                    state_machine_inputs,
                    log_store,
                    protocol_json_file
                ),
            ),
        ]

        # This coroutine is enabled by the passed in flag
        if self.open_browser:
            app["background_tasks"].append(
                app.loop.create_task(
                    tasks.task_open_browser(
                        True
                    )
                ),
            )

        # Add a reader (asynchronous interrupt) to the event loop for UART serial data
        # self._add_reader_process_uart(packet_queue)

    def _add_reader_process_uart(self, packet_queue):
        """
        Registers an asynchronous callback for when the UART receives bytes
        @param packet_queue : Queue to add logging packets to
        @note               : Needs testing on Jetson
        """
        # UART possible ports
        PORTS = {
            "PORT0" : "/dev/ttyTHS0",
            "PORT1" : "/dev/ttyTHS1",
            "PORT2" : "/dev/ttyTHS2",
        }

        # UART baud rate
        BAUD_RATE = 115200

        if sys.platform == "linux":
            UART = serial.Serial(
                PORTS["PORT1"],
                BAUD_RATE,
                timeout=1
            )

            current_packet = LogPacket()

            # Add reader to loop
            self.app.loop.add_reader(
                UART,          # Serial file descriptor
                process_uart,  # Callback
                packet_queue,  # Callback argument 1
                UART,          # Callback argument 2
                current_packet # Callback argument 3
            )
