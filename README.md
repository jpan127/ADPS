<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
### ADPS-Firmware

    Firmware for the ESP32 microcontroller

### Tasks

logger

    Logs useful information from all modules

rx

    Pool of tasks to service incoming client connections
    
tx

    Pool of client tasks to connect to a remote server

navigation

    Handles driving the motors of the wheels of the robot
=======
## Sidewalk Recognition

### Using OpenCV Version XXX
    JP is working on a C++ version
    Avi is working on a Python version

### Algorithms Used
    GaussianBlur
    Canny
    HoughLinesP

@note we need to choose a single language and stick with it
@note we need to take better pictures and use the same data
>>>>>>> sidewalk/master
=======
# Node.js Server Setup

----
## Setup

### Install Node.js
Download Node.js for your operating system at [https://nodejs.org/en/](https://nodejs.org/en/). 

### Install relevant packages

npm is node package manager, we will use it to install modules for our node.js project. We will use nodemon to run and watch for cahnges in our node application.

    npm install -g nodemon


----
### Clone Git Repository

The server code is hosted on Bitbucket. Clone it to a local location by navigating to the desired directory and using

    git clone https://InnovationSF@bitbucket.org/InnovationSF/goldengate.git

### Run the server

With all our dependencies installed, run the node server with 

    nodemon app.js

### Testing

Open a client and host window in separate windows/tabs by following the links on the home page. On the host window, allow the page to access your webcam. Streamed video can now be seen on a test window on the host page and on the client.

To test the directional buttons on the client page, click each button and observe the output in the terminal window.

----
## Current Status/Features

1. Bidirectional communication using Socket.io between Host and Client. 

Host -> Client: Video Stream

Client -> Host: Robot directional control

----
## Future Features

1. Link directional control to ESP32.
2. Link video stream from Jetson.

----
## Changelog
* 19-Oct-2017 Added README
>>>>>>> server/master
=======
# House Address Digit Detection

## Dataset
    SVHN (Street View House Numbers)
    http://ufldl.stanford.edu/housenumbers/
>>>>>>> nn/master
