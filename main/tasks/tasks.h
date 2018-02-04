#pragma once
// Project libraries
#include "common.h"



/// Struct for storing an ID to enumerate socket pool tasks, and a unique port number
typedef struct
{
    uint8_t task_id;    ///< Task ID for enumeration
    uint32_t port;      ///< Unique port number for each task
} socket_tx_task_params_S;

/// Initializes a single server socket
void init_server_socket(void);

/// Initialization functions
void init_task_navigation(void);
// void Init_RxTask(void);

/// Tasks
void task_navigation(void *p);
// void TxTask(void *p);
// void RxTask(void *p);