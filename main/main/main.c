#include "tasks.h"
#include "wifi.h"
#include "gpio.h"


/**
 *  @module      : main
 *  @description : Setup all drivers and tasks for the rtos scheduler
 */


#define STRINGIFY(s) (#s)
// No tasks need input parameters or handles, so simplify with an inline function
static inline void CREATE_TASK_LOW(TaskFunction_t  function, const uint32_t stack)  { xTaskCreate(function, STRINGIFY(function), stack, NULL, PRIORITY_LOW,  NULL); }
static inline void CREATE_TASK_MED(TaskFunction_t  function, const uint32_t stack)  { xTaskCreate(function, STRINGIFY(function), stack, NULL, PRIORITY_MED,  NULL); }
static inline void CREATE_TASK_HIGH(TaskFunction_t function, const uint32_t stack)  { xTaskCreate(function, STRINGIFY(function), stack, NULL, PRIORITY_HIGH, NULL); }

void app_main(void)
{
    /*/////////////////////////////
     *                            *
     *  Initialization Functions  *
     *                            *
     *//////////////////////////////

    // Initialize wifi
    init_wifi();

    // Initialize server socket
    init_server_socket();

    // Initialize gpios
    gpio_init();

    // Init_RxTask();
    // Init_ScissorTask();
    // Init_ServoTask();
    init_task_logger();
    init_task_navigation();

    // /*/////////////////////////////
    //  *                            *
    //  *     Client Thread Pool     *
    //  *                            *
    //  *//////////////////////////////

    // // Each task has its own port number + task ID
    // static tx_task_params_S tx_task_params[THREAD_POOL_SIZE] = { 0 };
    // // Each task has a unique name, postfixed by its task ID
    // const char *tx_task_base_name = "TxTask";
    // char tx_task_names[THREAD_POOL_SIZE][16] = { 0 };
    // // Create task pool of tx_task_params to transmit packets to remote server
    // for (int i=0; i<THREAD_POOL_SIZE; i++)
    // {
    //     const char task_id = i + '0';
    //     strncpy(tx_task_names[i], tx_task_base_name, strlen(tx_task_base_name));
    //     strcat(tx_task_names[i], &task_id);
    //     tx_task_params[i].task_id = i;
    //     tx_task_params[i].port    = CLIENT_PORT + i;
    //     xTaskCreate(&TxTask, 
    //                 tx_task_names[i], 
    //                 3000, 
    //                 (void *)(&tx_task_params[i]), 
    //                 PRIORITY_LOW, 
    //                 NULL);
    // }

    // /*/////////////////////////////
    //  *                            *
    //  *     Low Priority Tasks     *
    //  *                            *
    //  *//////////////////////////////

    CREATE_TASK_LOW(task_navigation, _4KB);
    // CREATE_TASK_LOW(ScissorTask,    _4KB);
    // CREATE_TASK_LOW(ServoTask,      _4KB);

    // /*/////////////////////////////
    //  *                            *
    //  *     Med Priority Tasks     *
    //  *                            *
    //  *//////////////////////////////

    // CREATE_TASK_LOW(RxTask,         _4KB);

    // /*//////////////////////////////
    //  *                             *
    //  *     High Priority Tasks     *
    //  *                             *
    //  *///////////////////////////////

}