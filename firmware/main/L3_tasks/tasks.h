#pragma once
// Project libraries
#include "common.h"



/// Struct for storing an ID to enumerate socket pool tasks, and a unique port number
typedef struct
{
    uint8_t task_id;    ///< Task ID for enumeration
    uint32_t port;      ///< Unique port number for each task
} task_tx_params_S;

typedef void * task_param_T;

/*/////////////////////////////
 *                            *
 *   General Initialization   *
 *                            *
 *////////////////////////////*/

/// Initializes a single server socket
void init_server_socket(void);

/// Creates all queues
void init_create_all_queues(void);

/// Stores a task handle so that each task can be referenced later
void register_task_handle(TaskHandle_t handle);

/// Grabs the task handles and the number of tasks created
void tasks_get_task_handles(TaskHandle_t ** handles, size_t * size);

/// Wrapper function over [xTaskCreate] that also registers the task handle
void rtos_create_task(TaskFunction_t function, const char * name, const uint32_t stack, rtos_priority_E priority);
void rtos_create_task_with_params(TaskFunction_t function, const char * name, const uint32_t stack, rtos_priority_E priority, task_param_T params);

/*/////////////////////////////
 *                            *
 *    Tasks Initialization    *
 *                            *
 *////////////////////////////*/

void init_task_logger(void);
void init_task_navigation(void);
void init_task_servo(void);

/*/////////////////////////////
 *                            *
 *            Tasks           *
 *                            *
 *////////////////////////////*/

void task_navigation(task_param_T params);

/**
 *  A pool of [task_tx] receives packets from a queue and sends it out to the server
 *      1. Receives a packet from [MessageTxQueue]
 *      2. Sends packet out
 *  @param : A [task_tx_params_S] struct of the task ID and the port number to create a client socket with
 *  @note  : All print statements will be sent out over this task, unless [TESTING] is defined
 *           then it will just print to terminal
 */
void task_tx(task_param_T params);

/**
 *  A pool of [task_rx] receives packets from lwip and services the command
 *      1. Receives packet from lwip
 *      2. Services the command 
 *  @param : A task ID (uint32_t)
 */
void task_rx(task_param_T params);

/**
 *  Controls the servo that swivels the camera left and right
 *      1. Waits for servo position command from server
 *      2. Adjusts the servo position
 *      3. Sends servo position acknowledge to the server
 */
void task_servo(task_param_T params);

/**
 *  Logs all useful information about the system over sockets to the server
 *  A series of multiplexed logging data is sent out, spread out evenly over the course of a second
 *  Muxes included:
 *      - mux_client
 *      - mux_server
 *      - mux_packet
 *      - mux_motor
 *      - mux_wifi
 *      - mux_task_watermarks
 */
void task_logger(task_param_T params);

/**
 *  Reads the infrared sensor through the ADC module
 *  @param : [gpio_E] GPIO number that is connected to the sensor
 */
void task_detection(task_param_T params);