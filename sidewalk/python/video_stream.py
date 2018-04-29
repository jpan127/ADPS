import sys
sys.path.append('/usr/local/lib/python2.7/site-packages')
import argparse
import numpy as np
import cv2
import math
import random
import json


from time import sleep, time

""" python video_stream.py -v """

#===================================#
#           Configuration           #
#===================================#
SHOW_GRIDS = False                  #
LINE       = True                   #
FILTER     = True                   #
PAUSE      = False                  #
#===================================#

# X and Y position variables used to sent directions to the ESP32
xpos = 0
ypos = 0

# Degrees to the left or right of zero the robot should go (+ is left, - is right)
direction_angle = 90
percentage_accuracy = 0


# RGB colors
BLACK = (0  ,   0,   0)
WHITE = (255, 255, 255)
RED   = (0  ,   0, 255)
GREEN = (0  , 255,   0)
BLUE  = (255,   0,   0)


#=================================================================================================#
#                      Step 1 : Find all relevant edges and create lines                          #
#=================================================================================================#

"""
@param      line: Tuple of two points, which are also tuples of (x, y)
@returns    Theta angle of line calculated from the slope, and the slope
"""
def calculate_line_angle(img, line):
    MARGIN_PERCENT = 0.15

    rise = line[1][1] - line[0][1]
    run  = line[1][0] - line[0][0]

    # Avoid divide by zero, run of zero is just a vertical line anyways
    if run == 0:
        return 0
    else:
        # Calculate slope and y-intercept: b = y - mx
        # The y-intercept is defined as the left side of the frame
        slope       = rise / (run * 1.0)
        y_intercept = line[0][1] - slope * line[0][0]

        # Calculate vertical middle of frame
        y_middle = img.shape[0] / 2
        margin   = img.shape[0] * MARGIN_PERCENT

        # If line intersects the y-intercept near the middle, ignore line
        if abs(y_intercept - y_middle) < margin:
            return 0
        # Calculate theta of angle and return
        else:
            theta = math.degrees(math.atan(slope))
            return theta

"""
@param      img: Input un-filtered frame
@returns    List of lines made by a tuple of two points which are also tuples of (x, y)
"""
def hough_transform(img):
    # These parameters change performance
    CANNY_THRESHHOLD = 50
    HOUGH_THRESHHOLD = 75

    # Gray
    gray    = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # Erode and Dilate
    kernel  = np.ones((15, 15), np.uint8)
    opening = cv2.morphologyEx(gray, cv2.MORPH_OPEN, kernel)
    # Canny Edge Detect
    edges   = cv2.Canny(opening, CANNY_THRESHHOLD, 150, apertureSize=3)
    # Hough transform
    lines   = cv2.HoughLines(edges, 1, np.pi/180, HOUGH_THRESHHOLD, 150)

    hough_lines = []

    if lines is None:
        # print " +++++++++ NONETYPE"
        return hough_lines

    for line in lines:
        rho   = line[0][0]
        theta = line[0][1]
        a     = math.cos(theta)
        b     = math.sin(theta)
        x0    = a * rho
        y0    = b * rho
        pt1   = ( int(x0+1000*(-b)), int(y0+1000*(a)) )
        pt2   = ( int(x0-1000*(-b)), int(y0-1000*(a)) )
        angle = calculate_line_angle(img, (pt1, pt2))

        # Only factor in lines with angles
        if 10 < abs(angle) < 75:
            if LINE:
                if angle <= 15:
                    color = RED
                elif angle <= 45:
                    color = BLUE
                else:
                    color = GREEN
                cv2.line(img, pt1, pt2, color, 2, cv2.LINE_AA)
            hough_lines.append((pt1, pt2))

    return hough_lines

#=================================================================================================#
#                               Step 2 : Grab a random sample of lines                            #
#=================================================================================================#

"""
@param      lines: List of all lines found in hough_transform
@param      size:  Number of samples to draw from
@returns    A list of random samples drawn from the list of lines
"""
def sample_lines(lines, size):
    # Make sure size is not larger than number of lines
    if size > len(lines):
        size = len(lines)
    return random.sample(lines, size)

#=================================================================================================#
#                               Step 3 : Find points of intersections                             #
#=================================================================================================#

def det(a, b):
    return a[0] * b[1] - a[1] * b[0]

"""
@param  line1:  The first line
@param  line2:  The second line
@returns        Nothing if no intersection.  Otherwise an x, y coordinate of the intersection.
"""
def find_line_intersection(line1, line2):
    x_diff = (line1[0][0] - line1[1][0], line2[0][0] - line2[1][0])
    y_diff = (line1[0][1] - line1[1][1], line2[0][1] - line2[1][1])
    div    = det(x_diff, y_diff)

    # Lines don't cross
    if div == 0:
        return None
    # Lines cross
    else:
        d = (det(*line1), det(*line2))
        x = det(d, x_diff) / div
        y = det(d, y_diff) / div
        # print line1, line2, x_diff, y_diff, div, d, x, y
        return x, y

"""
@param lines:   List of all lines found
@param img:     Input image frame
@returns        List of all intersections found
"""
def find_intersections(lines, img):
    intersections = []
    for i in range(len(lines)):
        for j in range(i + 1, len(lines)):
            if lines[i] != lines[j]:
                intersection = find_line_intersection(lines[i], lines[j])
                if intersection:
                    intersections.append(intersection)
    return intersections

#=================================================================================================#
#                   Step 4 : Find vanishing point from grid with most intersections               #
#=================================================================================================#

"""
@param img:             Input image frame
@param grid_size:       Size of each grid to divide the frame by
@param intersections:   List of intersection coordinates to find in grids
@returns                Cell with most intersections
"""
def find_vanishing_point(img, grid_size, intersections):
    # Image dimensions
    image_height = img.shape[0]
    image_width  = img.shape[1]

    # Grid dimensions
    grid_rows    = (image_height // grid_size) + 1
    grid_columns = (image_width  // grid_size) + 1

    # Current cell with most intersection points
    max_intersections = 0
    total_intersections = len(intersections)
    top_cell          = [0, 0, 0, 0]

    global xpos, ypos, percentage_accuracy

    for col in range(grid_columns):
        for row in range(grid_rows):
            cell_left   = col     * grid_size
            cell_right  = (col+1) * grid_size
            cell_bottom = row     * grid_size
            cell_top    = (row+1) * grid_size

            # Only show gridlines if enabled
            if SHOW_GRIDS:
                cv2.rectangle(img, (cell_left, cell_bottom), (cell_right, cell_top), WHITE, 2)

            # Count number of intersections in the current cell
            current_intersections = 0
            for x, y in intersections:
                if cell_left < x < cell_right and cell_bottom < y < cell_top:
                    current_intersections += 1

            # Find cell with most intersections
            if current_intersections > max_intersections:
                max_intersections = current_intersections
                top_cell = (cell_left, cell_right, cell_bottom, cell_top)

    if top_cell != [0, 0, 0, 0]:
        # Change the name to separate
        rx1 = int( (top_cell[0] + top_cell[1]) / 2 - grid_size / 2 )
        ry1 = int( (top_cell[2] + top_cell[3]) / 2 - grid_size / 2 )
        rx2 = int( (top_cell[0] + top_cell[1]) / 2 + grid_size / 2 )
        ry2 = int( (top_cell[2] + top_cell[3]) / 2 + grid_size / 2 )
        xpos = (rx1 + rx2) / 2
        ypos = (ry1 + ry2) / 2
        cv2.rectangle(img, (rx1, ry1), (rx2, ry2), BLACK, 2)
    percentage_accuracy = (100.0*max_intersections)/total_intersections
    # print "Max Intersections: ", max_intersections
    # print "Total Intersections: ", total_intersections
    # print "Average ", percentage_accuracy
    return top_cell

"""
Top level function
@param img: Input image frame
@returns    Output image frame with the vanishing point marked
"""
average_times = [
    ["Hough"              , 0],
    ["Sampling"           , 0],
    ["Find Intersections" , 0],
    ["Find Vantage Point" , 0],
    ["Total"              , 0]
]
counter = 0
def show_vanishing_point(img):
    global average_times, counter

    time1 = time()
    hough_lines = hough_transform(img)
    time2 = time()
    if hough_lines:
        random_sample = sample_lines(hough_lines, 100)
        time3 = time()
		#Force 2 boundry lines to houghline
        random_sample.append(((1,1),(1,img.shape[0]-1)))
        random_sample.append(((img.shape[1] - 1, 1),(img.shape[1] - 1, img.shape[0] - 1)))
        cv2.line(img, (0,0),(0,img.shape[0]), WHITE, 2, cv2.LINE_AA)
        cv2.line(img, (img.shape[1], 0),(img.shape[1], img.shape[0]), WHITE, 2, cv2.LINE_AA)
		#----------------------------------
        intersections = find_intersections(random_sample, img)
        time4 = time()
        if intersections:
            grid_size = min(img.shape[0], img.shape[1]) // 15
            time5 = time()
            vanishing_point = find_vanishing_point(img, grid_size, intersections)
            time6 = time()

            average_times[0][1] += time2 - time1
            average_times[1][1] += time3 - time2
            average_times[2][1] += time4 - time3
            average_times[3][1] += time6 - time5
            average_times[4][1] += time6 - time1
            counter += 1

    return img

def show_sidewalk(img):
    mask = cv2.GaussianBlur(img, (7, 7), 0)
    mask = cv2.inRange(mask, (160, 0, 0), (255, 255, 255))
    return mask


"""
@param image_width:     Width of image capture region

"""
def send_directions(image_width, output_image):
    # Current coordinates of intersection point
    global xpos, ypos, percentage_accuracy
    global direction_angle

    # print "Xraw = ", xpos, " Yraw = ", ypos

    if (xpos <= 0) or (ypos <= 0):
        print "No valid vantage point"
    else:
        xpos = xpos - image_width/2
        ypos = -1*(ypos - image_width)

        # print "Xadj = ", xpos, " Yadj = ", ypos
        angle = math.atan2(ypos, xpos)
        angle = angle * (180 / math.pi)
        direction_angle = (2 * direction_angle + angle) / 3
        direction_angle_s = float('%.3f' % (direction_angle))
        percentage_accuracy_s = float('%.3f' % (percentage_accuracy))
        # print "Angle = ",direction_angle,"*"
        
        if direction_angle < 75:
            print "RIGHT"
        elif direction_angle < 105:
            print "STRAIGHT"
        else:
            print "LEFT"

        dictionary = {
            'angle': direction_angle_s,
            'accuracy': percentage_accuracy_s,
        }
        
        # print(json.dumps(dictionary))
        with open('data.txt', 'w') as outfile:  
            json.dump(dictionary, outfile, indent=4)

        cv2.imwrite("img.jpg",output_image)
        # print "Raw Angle = ",angle,"*"
    


""" OpenCV main loop to parse image frames """
def main():
    """
    https://github.com/SZanlongo/vanishing-point-detection/blob/master/src/Testing.py

    show_vanishing_point
        \ hough_transform
          sample_lines
          find_intersections
          find_vanishing_point
    """

    # Global config variables
    global SHOW_GRIDS, LINE, FILTER, PAUSE
    # Coordinates of intersection point
    global xpos, ypos

    # Parse arguments
    ap = argparse.ArgumentParser()
    ap.add_argument("-v", "--video", help = "path to the (optional) video file")
    args = vars(ap.parse_args())

    if not args.get("video", False):
        # print "Going for camera"
        camera = cv2.VideoCapture(1)
        camera.set(3, 640)
        camera.set(4, 480)
        camera.set(5, 5)
        # camera1 = cv2.videooCapture(1)
        # camera1.set(3, 640)
        # camera1.set(4, 480)
        # camera1.set(5, 10)
    else:
        camera = cv2.VideoCapture(args["video"])
        camera.set(3, 640)
        camera.set(4, 480)

    # Main Loop
    while True:
        for x in xrange(1,10):
            # Read frame
            (grabbed, frame) = camera.read()
            if not grabbed:
                break

            # Apply filters and show frame
            if FILTER:
                vantage_frame  = frame.copy()
                sidewalk_frame = frame.copy()
                vantage_frame = show_vanishing_point(vantage_frame)
                sidewalk_frame = show_sidewalk(sidewalk_frame)
                send_directions(frame.shape[1], frame)
                cv2.imshow("Sidewalk1", sidewalk_frame)
                cv2.imshow("Frame1", vantage_frame)
                # sleep(1.0)
            else:
                cv2.imshow("Frame", frame)

            # Parse key triggers
            key = cv2.waitKey(1) & 0xFF
            if key == ord("q"):
                break
            elif key == ord("g"):
                SHOW_GRIDS = not SHOW_GRIDS
                print "Grids ON" if SHOW_GRIDS else "Grids OFF"
                sys.stdout.flush()
            elif key == ord("l"):
                LINE = not LINE
                print "Lines ON" if LINE else "Lines OFF"
                sys.stdout.flush()
            elif key == ord("f"):
                FILTER = not FILTER
                print "Filtering ON" if FILTER else "Filtering OFF"
                sys.stdout.flush()
            elif key == ord("p"):
                PAUSE = not PAUSE
                print "Pause ON" if PAUSE else "Pause OFF"
                sys.stdout.flush()
                if PAUSE:
                    while cv2.waitKey(1) != ord("p"):
                        pass
        # # Read frame
        # (grabbed1, frame1) = camera1.read()
        # if not grabbed1:
        #     break

        # # Apply filters and show frame
        # if FILTER:
        #     vantage_frame1  = frame1.copy()
        #     sidewalk_frame1 = frame1.copy()
        #     vantage_frame1 = show_vanishing_point(vantage_frame1)
        #     sidewalk_frame1 = show_sidewalk(sidewalk_frame1)
        #     send_directions(frame1.shape[1])
        #     cv2.imshow("Sidewalk", sidewalk_frame1)
        #     cv2.imshow("Frame", vantage_frame1)
        #     # sleep(0.25)
        # else:
        #     cv2.imshow("Frame", frame1)

    # Close
    camera.release()
    cv2.destroyAllWindows()

    # Profile slowest operations
    # decimal_places = 6
    # power = math.pow(10, decimal_places)
    # print '-' * 80
    # for time in average_times:
    #     print time[0].ljust(20), '{0:f}'.format(math.ceil(time[1]/counter * power) / power)
    # print '-' * 80
    return

if __name__ == '__main__':
    main()

"""
Notes:
    When the sidewalk turns left or right, the vantage point is on the left or right.
    The robot needs to be able to understand it needs to go forward first before turning, 
    otherwise if it immediately turns it will go off the sidewalk.
        It could use a filter to mask everything but the sidewalk to know where to go in the immediate
        vicinity, and the vantage point to know what final direction to go.
"""