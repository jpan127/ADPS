#include "tasks.h"
#include "wifi.h"
#include "gpio.h"



/**
 *  @module      : main
 *  @description : Setup all drivers and tasks for the rtos scheduler
 */

/// Stringifies something
#define STRINGIFY(s) (#s)

// Each task has its own port number + task ID
static task_tx_params_S task_tx_params[THREAD_POOL_SIZE] = { 0 };

/// No tasks need input parameters or handles, so simplify with an inline function
static inline void CREATE_TASK_LOW(TaskFunction_t  function, const uint32_t stack)  { xTaskCreate(function, STRINGIFY(function), stack, NULL, PRIORITY_LOW,  NULL); }
static inline void CREATE_TASK_MED(TaskFunction_t  function, const uint32_t stack)  { xTaskCreate(function, STRINGIFY(function), stack, NULL, PRIORITY_MED,  NULL); }
static inline void CREATE_TASK_HIGH(TaskFunction_t function, const uint32_t stack)  { xTaskCreate(function, STRINGIFY(function), stack, NULL, PRIORITY_HIGH, NULL); }

static void create_tx_thread_pool(void)
{
    // Each task has a unique name, postfixed by its task ID
    const char * const task_tx_base_name = "task_tx";
    char task_tx_names[THREAD_POOL_SIZE][16] = { 0 };
    const uint16_t stack_size = 3000;

    // Create task pool of task_tx_params to transmit packets to remote server
    for (int i=0; i<THREAD_POOL_SIZE; i++)
    {
        const char task_id = i + '0';
        strncpy(task_tx_names[i], task_tx_base_name, strlen(task_tx_base_name));
        strcat(task_tx_names[i], &task_id);
        task_tx_params[i].task_id = i;
        task_tx_params[i].port    = CLIENT_PORT + i;
        xTaskCreate(&task_tx, 
                    task_tx_names[i], 
                    stack_size, 
                    (void *)(&task_tx_params[i]), 
                    PRIORITY_LOW, 
                    NULL);
    }
}

// static void create_rx_thread_pool(void)
// {
//     // Each task has a unique name, postfixed by its task ID
//     const char * const task_tx_base_name = "task_tx";
//     char task_tx_names[THREAD_POOL_SIZE][16] = { 0 };
//     const uint16_t stack_size = 3000;

//     // Create task pool of task_tx_params to transmit packets to remote server
//     for (int i=0; i<THREAD_POOL_SIZE; i++)
//     {
//         const char task_id = i + '0';
//         strncpy(task_tx_names[i], task_tx_base_name, strlen(task_tx_base_name));
//         strcat(task_tx_names[i], &task_id);
//         task_tx_params[i].task_id = i;
//         task_tx_params[i].port    = CLIENT_PORT + i;
//         xTaskCreate(&task_tx, 
//                     task_tx_names[i], 
//                     stack_size, 
//                     (void *)(&task_tx_params[i]), 
//                     PRIORITY_LOW, 
//                     NULL);
//     }
// }

/// MAIN
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
    init_task_logger();
    init_task_navigation();

    /*/////////////////////////////
     *                            *
     *     Client Thread Pool     *
     *                            *
     *//////////////////////////////

    create_tx_thread_pool();

    /*/////////////////////////////
     *                            *
     *     Low Priority Tasks     *
     *                            *
     *//////////////////////////////

    CREATE_TASK_LOW(task_navigation, _4KB);

    /*/////////////////////////////
     *                            *
     *     Med Priority Tasks     *
     *                            *
     *//////////////////////////////

    // CREATE_TASK_LOW(RxTask,         _4KB);

    /*//////////////////////////////
     *                             *
     *     High Priority Tasks     *
     *                             *
     *///////////////////////////////

}