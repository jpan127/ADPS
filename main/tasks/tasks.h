#pragma once
// Project libraries
#include "common.h"


// Macro to ensure each task has the same prototype
// Is this really necessary?
#define TASK_PROTOTYPE(name) void name(void *p)
#define INIT_PROTOTYPE(name) void name(void)

typedef struct
{
    uint8_t task_id;
    uint32_t port;
} socket_tx_task_params_S;

void init_server_socket(void);

// Initialization functions
INIT_PROTOTYPE(Init_RxTask);
INIT_PROTOTYPE(Init_ScissorTask);
INIT_PROTOTYPE(Init_ServoTask);
INIT_PROTOTYPE(Init_NavigationTask);

// Tasks
TASK_PROTOTYPE(NavigationTask);
TASK_PROTOTYPE(ScissorTask);
TASK_PROTOTYPE(ServoTask);
TASK_PROTOTYPE(TxTask);
TASK_PROTOTYPE(RxTask);