#pragma once
// Project libraries
#include "common.h"



/// Struct for storing an ID to enumerate socket pool tasks, and a unique port number
typedef struct
{
    uint8_t task_id;    ///< Task ID for enumeration
    uint32_t port;      ///< Unique port number for each task
} task_tx_params_S;

/// Initializes a single server socket
void init_server_socket(void);

/// Creates all queues
void init_create_all_queues(void);

/*/////////////////////////////
 *                            *
 *    Tasks Initialization    *
 *                            *
 *////////////////////////////*/

void init_task_logger(void);
void init_task_navigation(void);

/*/////////////////////////////
 *                            *
 *            Tasks           *
 *                            *
 *////////////////////////////*/

void task_navigation(void *p);

/**
 *  A pool of [task_tx] receives packets from a queue and sends it out to the server
 *      1. Receives a packet from [MessageTxQueue]
 *      2. Sends packet out
 *  @note : All print statements will be sent out over this task, unless [TESTING] is defined
 *          then it will just print to terminal
 */
void task_tx(void *p);

void task_rx(void *p);