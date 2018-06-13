# APDS : Autonomous Postal Delivery System

----
### Firmware

    Firmware for the ESP32 microcontroller

##### Tasks
| Task       | Description                                              |
|------------|----------------------------------------------------------|
| logger     | Logs useful information from all modules                 |
| rx         | Pool of tasks to service incoming client connections     |
| tx         | Pool of client tasks to connect to a remote server       |
| navigation | Services wheel control / navigation commands             |
| detection  | Periodically reads from the infrared sensors             |
| self_test  | Executes a self test to check if everything is working   |
| servo      | Controls the servo that is attached to the side camera   |

----
### Sidewalk Recognition

An application utilizing OpenCV is responsible for analyzing and filtering an image frame to detect the lines of interest.
Each image frame is then split into sub-grids.  Intersections of all lines are calculated and the sub-grid with the global maximum of intersections
is then chosen as location of the vantage point.  From this information, the system understands where the path / sidewalk is leading towards and the
system can adjust navigation accordingly.

----
### Node.js Server Setup

##### Setup

#####  Install Node.js
Download Node.js for your operating system at [https://nodejs.org/en/](https://nodejs.org/en/). 

#####  Install relevant packages

npm is node package manager, we will use it to install modules for our node.js project. We will use nodemon to run and watch for cahnges in our node application.

    npm install -g nodemon

##### Run the server

With all our dependencies installed, run the node server with 

    nodemon app.js

##### Testing

Open a client and host window in separate windows/tabs by following the links on the home page. On the host window, allow the page to access your webcam. Streamed video can now be seen on a test window on the host page and on the client.

To test the directional buttons on the client page, click each button and observe the output in the terminal window.

##### Current Status/Features

1. Bidirectional communication using Socket.io between Host and Client. 

Host -> Client: Video Stream

Client -> Host: Robot directional control

##### Future Features

1. Link directional control to ESP32.
2. Link video stream from Jetson.

##### Changelog
* 19-Oct-2017 Added README

----
### House Address Digit Detection

##### Dataset
    SVHN (Street View House Numbers)
    http://ufldl.stanford.edu/housenumbers/
