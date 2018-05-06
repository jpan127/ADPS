from time import sleep

class RobotStates(Enum):
    NAVIGATING_SIDEWALK  = 0
    LOOKING_FOR_PATH     = 1
    FOUND_PATH_TO_HOUSE  = 2
    NAVIGATING_PATH      = 3
    REACHED_DOOR         = 4
    DELIVER_PACKAGE      = 5


ONE_SECOND = 1
DELAY_250_MS = ONE_SECOND / 4
HOUSE_ADDRESS_NUMBER = 1234

def handle_navigation():
    call_opencv_script()    # Saves 2 images to /camera directory + save information to a JSON file about vantage point of both cameras
    with open("data.json", "r") as file:
        # Load the data into a data structure
        data = json.load(file)
        
        # Read the data
        degrees = data["degrees"]
        confidence = data["confidence"]
        
        # Determine what to do
        if degrees < 0:
            send_command_go_left(degrees)
        elif degrees > 0:
            send_command_go_right(degrees)


def state_machine():

    state = RobotStates.NAVIGATING_SIDEWALK

    # Forever loop
    while True:

        if state == RobotStates.NAVIGATING_SIDEWALK:

            while True:
                handle_navigation()
                call_inference_script() # Runs inference on /camera/left.jpeg + /camera/right.jpeg
                with open("data.json", "r") as file:
                    # Load the data into a data structure
                    data = json.load(file)

                    # Read the data
                    number = data["number"]
                    if number == HOUSE_ADDRESS_NUMBER:
                        state = RobotStates.LOOKING_FOR_PATH
                        break
                sleep(DELAY_250_MS)

        elif state == RobotStates.LOOKING_FOR_PATH:

            while True:
                handle_navigation()
                if found_vantage_point():
                    state = RobotStates.FOUND_PATH_TO_HOUSE
                    break

        elif state == RobotStates.FOUND_PATH_TO_HOUSE:

            while True:
                handle_navigation()
                if received_feedback_from_ir():
                    state = RobotStates.NAVIGATING_PATH
                    break

        elif state == RobotStates.NAVIGATING_PATH:
        elif state == RobotStates.REACHED_DOOR:
        elif state == RobotStates.DELIVER_PACKAGE:
        else:
            print("State not recognized : {}".format(state))