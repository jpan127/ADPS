#include "tasks.h"
// Project libraries
#include "server.h"
#include "packet.h"
#include "repeater.h"



/// Extern
QueueHandle_t     MessageRxQueue      = NULL;
QueueHandle_t     MessageTxQueue      = NULL;
SemaphoreHandle_t SelfTestSemaphore   = NULL;
SemaphoreHandle_t DeliverySemaphore   = NULL;
SemaphoreHandle_t NavigationSemaphore = NULL;
SemaphoreHandle_t ServoSemaphore      = NULL;

/// Size of message queues
static const uint8_t message_queue_size = 50;

/// Array of task contexts
static rtos_task_context_block_S TCBs[MAX_TASKS] = { 0 };

/// Handle number, array index pointer for [TCBs]
static uint8_t num_tasks = 0;

/// Start off suspended
static bool suspended = true;

/// Quick hack because RTOS is not setting priorities straight
void set_suspend_state(const bool on)
{
    suspended = on;
}

bool get_suspend_state(void)
{
    return suspended;
}

static bool attempt_to_init_server_socket(void)
{
    const uint8_t queue_size = 5;
    return tcp_server_init(SERVER_PORT, queue_size);
}

void init_server_socket(void)
{
    const repeat_S repeat_task =
    {
        .num_retries = 10,
        .delay_ms    = 1000,
        .callback    = &attempt_to_init_server_socket,
    };

    if (!repeater_execute(&repeat_task))
    {
        ESP_LOGE("init_server_socket", "FAILED to initialize server");
    }
    else
    {
        xEventGroupSetBits( StatusEventGroup, BIT_SERVER_READY );
        ESP_LOGI("init_server_socket", "SUCCESSFULLY initialized server");
    }
}

void init_create_all_queues(void)
{
    MessageRxQueue = xQueueCreate(message_queue_size, sizeof(command_packet_S));
    MessageTxQueue = xQueueCreate(message_queue_size, sizeof(diagnostic_packet_S));

    // Check for failure
    if (!MessageRxQueue) { ESP_LOGE("init_create_all_queues", "MessageRxQueue failed to create"); }
    if (!MessageTxQueue) { ESP_LOGE("init_create_all_queues", "MessageTxQueue failed to create"); }
}

void init_create_all_semaphores(void)
{
    // Create semaphores
    vSemaphoreCreateBinary(SelfTestSemaphore);
    vSemaphoreCreateBinary(DeliverySemaphore);
    vSemaphoreCreateBinary(NavigationSemaphore);
    vSemaphoreCreateBinary(ServoSemaphore);

    // Check for failure
    if (!SelfTestSemaphore)   { ESP_LOGE("init_create_all_semaphores", "SelfTestSemaphore failed to create");   }
    if (!DeliverySemaphore)   { ESP_LOGE("init_create_all_semaphores", "DeliverySemaphore failed to create");   }
    if (!NavigationSemaphore) { ESP_LOGE("init_create_all_semaphores", "NavigationSemaphore failed to create"); }
    if (!ServoSemaphore)      { ESP_LOGE("init_create_all_semaphores", "ServoSemaphore failed to create");      }

    // Take because these tasks should not start before receiving commands
    xSemaphoreTake(DeliverySemaphore   , NO_DELAY);
    xSemaphoreTake(NavigationSemaphore , NO_DELAY);
    xSemaphoreTake(ServoSemaphore      , NO_DELAY);
}

static void register_task_handle(TaskHandle_t handle, const size_t size)
{
    if (num_tasks >= MAX_TASKS)
    {
        ESP_LOGE("register_task_handle", "Task creation has exceeded maximum number of allowed tasks %d", MAX_TASKS);
    }
    else
    {
        ESP_LOGI("register_task_handle", "Registering %d for %p", num_tasks, handle);

        TCBs[num_tasks] = (rtos_task_context_block_S)
        {
            .handle     = handle,
            .stack_size = size,
            .task_id    = num_tasks,
        };

        num_tasks++;
    }
}

void tasks_get_tcbs(rtos_task_context_block_S ** handles, size_t * size)
{
    *handles = &TCBs[0];
    *size    = num_tasks;
}

void rtos_create_task(TaskFunction_t function, const char * name, const uint32_t stack, const rtos_priority_E priority)
{
    TaskHandle_t handle = NULL;

    xTaskCreate(
        function,
        name,
        stack,
        NULL,
        (uint32_t)priority,
        &handle
    );

    register_task_handle(handle, stack);
}

void rtos_create_task_with_params(TaskFunction_t function, const char * name, const uint32_t stack, const rtos_priority_E priority, task_param_T params)
{
    TaskHandle_t handle = NULL;

    xTaskCreate(
        function,
        name,
        stack,
        params,
        (uint32_t)priority,
        &handle
    );

    register_task_handle(handle, stack);
}
