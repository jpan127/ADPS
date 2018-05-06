#include "tasks.h"
#include "cmd_handler.h"
#include "motor.h"
#include "navigation.h"



/// Externed semaphore
SemaphoreHandle_t self_test_sem = NULL;

static void execute_self_test_routine(void)
{
    DISABLE_EXTERNAL_COMMANDS();
    {
        ESP_LOGI("SELF_TEST", "Part 1: Testing servo...");
        {
            for (uint8_t duty = 0; duty < 180; duty++)
            {
                motor_move(motor_servo, motor_dir_a_forward, (float)duty);
                DELAY_MS(10);
            }

            for (int8_t duty = 180; duty >= 0; duty--)
            {
                motor_move(motor_servo, motor_dir_a_forward, (float)duty);
                DELAY_MS(10);
            }

            motor_stop(motor_servo);
        }

        ESP_LOGI("SELF_TEST", "Part 2: Testing wheels...");
        {
            ESP_LOGI("SELF_TEST", "\tTesting forward...");
            motor_move(motor_wheels, motor_dir_both_forward, 40.0f);
            motor_stop(motor_wheels);
            DELAY_MS(200);

            ESP_LOGI("SELF_TEST", "\tTesting backward...");
            motor_move(motor_wheels, motor_dir_both_backward, 40.0f);
            motor_stop(motor_wheels);
            DELAY_MS(200);
            
            ESP_LOGI("SELF_TEST", "\tTesting left pivot...");
            navigation_pivot_left();
            
            ESP_LOGI("SELF_TEST", "\tTesting right pivot...");
            navigation_pivot_right();

            ESP_LOGI("SELF_TEST", "\tTesting delivery forward...");
            motor_move(motor_delivery, motor_dir_delivery_forward, 100.0f);
            motor_stop(motor_delivery);
            DELAY_MS(2000);

            ESP_LOGI("SELF_TEST", "\tTesting delivery backward...");
            motor_move(motor_delivery, motor_dir_delivery_backward, 100.0f);
            motor_stop(motor_delivery);
            DELAY_MS(2000);
        }
    }
    ENABLE_EXTERNAL_COMMANDS();
}

void task_self_test(task_param_T params)
{
    // Initialize semaphore
    vSemaphoreCreateBinary(self_test_sem);

    DELAY_MS(1000);

    if (!self_test_sem)
    {
        ESP_LOGE("task_self_test", "Semaphore could not be created, suspending task...");
        vTaskSuspend(NULL);
    }

    // Main Loop
    while(1)
    {
        if (xSemaphoreTake(self_test_sem, MAX_DELAY))
        {
            execute_self_test_routine();
        }
    }
}