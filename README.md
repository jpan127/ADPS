# APDS : Autonomous Postal Delivery System

----
### Firmware

    Firmware for the ESP32 microcontroller

##### Tasks
|------------|----------------------------------------------------------|
| Task       | Description                                              |
|------------|----------------------------------------------------------|
| logger     | Logs useful information from all modules                 |
| rx         | Pool of tasks to service incoming client connections     |
| tx         | Pool of client tasks to connect to a remote server       |
| navigation | Services wheel control / navigation commands             |
| detection  | Periodically reads from the infrared sensors             |
| self_test  | Executes a self test to check if everything is working   |
| servo      | Controls the servo that is attached to the side camera   |
|------------|----------------------------------------------------------|

----
### Sidewalk Recognition

An application utilizing OpenCV is responsible for analyzing and filtering an image frame to detect the lines of interest.
Each image frame is then split into sub-grids.  Intersections of all lines are calculated and the sub-grid with the global maximum of intersections
is then chosen as location of the vantage point.  From this information, the system understands where the path / sidewalk is leading towards and the
system can adjust navigation accordingly.

----

##### Changelog
* 19-Oct-2017 Added README
* 01-Aug-2018 Removed ML and Node.JS info
