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
static uint8_t task_rx_params[16] = { 0 };

/// No tasks need input parameters or handles, so simplify with an inline function
static inline void CREATE_TASK_LOW(TaskFunction_t  function, const uint32_t stack)  { xTaskCreate(function, STRINGIFY(function), stack, NULL, PRIORITY_LOW,  NULL); }
static inline void CREATE_TASK_MED(TaskFunction_t  function, const uint32_t stack)  { xTaskCreate(function, STRINGIFY(function), stack, NULL, PRIORITY_MED,  NULL); }
static inline void CREATE_TASK_HIGH(TaskFunction_t function, const uint32_t stack)  { xTaskCreate(function, STRINGIFY(function), stack, NULL, PRIORITY_HIGH, NULL); }

/**
 *  Creates THREAD_POOL_SIZE many [task_tx]s
 */
static void create_tx_thread_pool(void)
{
    // Each task has a unique name, postfixed by its task ID
    const char * const task_tx_base_name = "task_tx";
    char task_tx_names[THREAD_POOL_SIZE][16] = { 0 };
    const uint16_t _12KB = 3000;

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
                    _12KB, 
                    (void *)(&task_tx_params[i]), 
                    PRIORITY_LOW, 
                    NULL);
    }
}

/**
 *  Creates THREAD_POOL_SIZE many [task_rx]s
 */
static void create_rx_thread_pool(void)
{
    // Each task has a unique name, postfixed by its task ID
    const char * const task_rx_base_name = "task_rx";
    char task_rx_names[THREAD_POOL_SIZE][16] = { 0 };
    const uint16_t _12KB = 3000;

    // Create task pool of task_tx_params to transmit packets to remote server
    for (int i=0; i<THREAD_POOL_SIZE; i++)
    {
        const char task_id = i + '0';
        strncpy(task_rx_names[i], task_rx_base_name, strlen(task_rx_base_name));
        strcat(task_rx_names[i], &task_id);
        task_rx_params[i] = i;
        xTaskCreate(&task_rx, 
                    task_rx_names[i], 
                    _12KB, 
                    (void *)(&task_rx_params[i]),
                    PRIORITY_LOW, 
                    NULL);
    }
}

/// MAIN
void app_main(void)
{
    /*/////////////////////////////
     *                            *
     *  Initialization Functions  *
     *                            *
     *//////////////////////////////

#if 0
    // Initialize wifi
    init_wifi();

    // Initialize server socket
    init_server_socket();
#endif
    
    // Initialize gpios
    gpio_init();

    // Create all queues
    init_create_all_queues();

    // Initialize tasks
    init_task_logger();
    init_task_navigation();
    init_task_servo();

    /*/////////////////////////////
     *                            *
     *     Low Priority Tasks     *
     *                            *
     *//////////////////////////////

    // create_tx_thread_pool();
    // create_rx_thread_pool();

    /*/////////////////////////////
     *                            *
     *     Med Priority Tasks     *
     *                            *
     *//////////////////////////////

    // CREATE_TASK_MED(task_navigation , _8KB);
    CREATE_TASK_MED(task_servo      , _8KB);

    /*//////////////////////////////
     *                             *
     *     High Priority Tasks     *
     *                             *
     *///////////////////////////////
}