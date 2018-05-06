#include "tasks.h"
#include "wifi.h"
#include "gpio.h"



/**
 *  @module : main
 *
 *  Setup all drivers and tasks for the rtos scheduler
 *  Initializes and allocates all queues, semaphores, and dynamically allocated memory first
 */

/// Maximum characters a task name can be
#define MAX_TASK_NAME_CHARS (16)

/// Each task has its own port number + task ID
static task_tx_params_S task_tx_params[THREAD_POOL_SIZE] = { 0 };
static uint8_t task_rx_params[THREAD_POOL_SIZE]          = { 0 };


/// @ { It is annoying to find the right tasks to enable during testing so just comment / uncomment these defines for now
    // #define TEST_TASK_TX
    #define TEST_TASK_RX
    #define TEST_TASK_SERVO
    // #define TEST_TASK_LOGGER
    #define TEST_TASK_DETECTION
    // #define TEST_TASK_DELIVERY
/// @ }

/**
 *  Creates a pool of either a client or a server task
 *  @param base_name  : The prefix of the task name, for example "client" would create names "client0" ... "client9"
 *  @param priority   : The priority of all of the tasks
 *  @param stack_size : The stack size of all of the tasks
 *  @param function   : A pointer to the task being created
 *  @param client     : A flag, true to say create client tasks, false for server tasks
 */
static void adps_create_task_thread_pool(const char * const base_name,
                                         const rtos_priority_E priority,
                                         const uint16_t stack_size,
                                         TaskFunction_t function,
                                         const bool client)
{
    char task_names[THREAD_POOL_SIZE][MAX_TASK_NAME_CHARS] = { 0 };

    for (uint8_t i = 0; i < THREAD_POOL_SIZE; i++)
    {
        // Set up the task's unique name and ID
        const char task_id = i + '0';
        strncpy(task_names[i], base_name, strlen(base_name));
        strcat(task_names[i], &task_id);

        // Single void pointer to point to the task params
        void * task_params = NULL;

        // Depending if it is client or server, initialize the params to pass to the task
        if (client)
        {
            task_tx_params[i].task_id = i;
            task_tx_params[i].port    = CLIENT_PORT + i;
            task_params               = (void *)(&task_tx_params[i]);
        }
        else
        {
            task_rx_params[i] = i;
            task_params       = (void *)(&task_rx_params[i]);
        }

        // Create the task
        rtos_create_task_with_params(
            function,
            task_names[i], 
            stack_size,
            priority,
            task_params
        );
    }
}                                         

/// Initializes the state of modules that are not tasks
static void adps_init_modules(void)
{
#if defined(TEST_TASK_TX) || defined(TEST_TASK_RX)
    // Initialize wifi
    init_wifi();

    // Initialize server socket
    init_server_socket();
#endif

    // Initialize gpios
    gpio_init();

    // Create all queues
    init_create_all_queues();
}

/// Initializes the state of tasks
static void adps_init_tasks(void)
{
    init_task_logger();

    init_task_navigation();

    init_task_servo();

    init_task_delivery();
}

/// Creates all low priority tasks
static void adps_create_low_priority_tasks(void)
{
#ifdef TEST_TASK_LOGGER
    rtos_create_task(&task_logger, "task_logger", _8KB , PRIORITY_LOW);
#endif
}

/// Creates all medium priority tasks
static void adps_create_medium_priority_tasks(void)
{
#ifdef TEST_TASK_TX
    // Create TX client threads
    adps_create_task_thread_pool("task_tx", PRIORITY_MED, _16KB, task_tx, true);
#endif

#ifdef TEST_TASK_RX
    // Create RX server threads
    adps_create_task_thread_pool("task_rx", PRIORITY_MED, _16KB, task_rx, false);
#endif

    // rtos_create_task(&task_navigation , "task_navigation" , _8KB , PRIORITY_MED);

#ifdef TEST_TASK_SERVO
    rtos_create_task(&task_servo, "task_servo", _8KB , PRIORITY_MED);
#endif

#ifdef TEST_TASK_DETECTION
    rtos_create_task_with_params(
        &task_detection, 
        "task_detection",
        _8KB,
        PRIORITY_MED,
        (void *)(gpio_adc_infrared_bottom)
    );
#endif

#ifdef TEST_TASK_DELIVERY
    rtos_create_task(&task_delivery, "task_delivery", _8KB, PRIORITY_MED);
#endif
}

/// Creates all high priority tasks
static void adps_create_high_priority_tasks(void)
{
    /// No high priority tasks so far
}

/// MAIN
void app_main(void)
{
    adps_init_modules();

    adps_init_tasks();

    adps_create_low_priority_tasks();

    adps_create_medium_priority_tasks();

    adps_create_high_priority_tasks();
}
