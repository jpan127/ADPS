# Public libraries
import asyncio
import argparse

# Project libraries
from server import server



def _handle_arguments():
    """
    Creates an argument parser, registers arguments, and parses them
    """
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument( "-b" , default = True , action = "store_true" , dest = "open_browser" , help = "Open browser on application start" )
    arg_parser.add_argument( "-s" , default = True , action = "store_true" , dest = "run_server"   , help = "Run the web server"                )
    arg_parser.add_argument( "-a" , default = True , action = "store_true" , dest = "run_app"      , help = "Run the state machine application" )
    return arg_parser.parse_args()


def main():
    """
    Application main entry point
    """
    args = _handle_arguments()
    try:
        apds_server = server.ApdsServer(
            args.run_server,
            args.run_app,
            args.open_browser
        )
    except KeyboardInterrupt:
        print("KeyboardInterrupt caught, exiting...")
        if args.run_app:
            event_loop = asyncio.get_event_loop()
            event_loop.run_until_complete(exit(1))


if __name__ == '__main__':
    main()
