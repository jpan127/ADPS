# Jetson Server

### Purpose
The Jetson TX2 is in charge of running multiple processes and maintaining a myriad of reponsibilities.  To organize these tasks together and to allow a browser-based visual, a Python server is responsible for servicing incoming requests, storing and fetching data, and internally executing a continuous state machine.

### Responsibilities
- Local web server
    - Browser should be able to connect to the server and have an interactive GUI for manual commands
    - Browser should be able to display metrics and logging information
- State machine
    - As soon as the system starts running, the state machine should be executing
    - Inputs come externally and asynchronously and the state machine should handle this
    - The state machine should be fault tolerant
- Communication
    - Should be asynchronously communicating with the external microcontroller through UART
