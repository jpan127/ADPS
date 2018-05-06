#include "tasks.h"
#include "cmd_handler.h"
#include "motor.h"



/// Externed semaphore
SemaphoreHandle_t self_test_sem = NULL;

static void execute_self_test_routine(void)
{

}

void task_self_test(task_param_T params)
{
    // Initialize semaphore
    vSemaphoreCreateBinary(self_test_sem);
    xSemaphoreGive(self_test_sem);

    DELAY_MS(1000);

    // Main Loop
    while(1)
    {
        if (xSemaphoreTake(self_test_sem, MAX_DELAY))
        {

        }
    }
}