# ADPS : Autonomous Postal Delivery System

----
### ADPS-Firmware

    Firmware for the ESP32 microcontroller

##### Tasks

logger

    Logs useful information from all modules

rx

    Pool of tasks to service incoming client connections
    
tx

    Pool of client tasks to connect to a remote server

navigation

    Handles driving the motors of the wheels of the robot

----
### Sidewalk Recognition

##### Using OpenCV Version XXX
    JP is working on a C++ version
    Avi is working on a Python version

##### Algorithms Used
    GaussianBlur
    Canny
    HoughLinesP

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
