#include "tasks.h"
#include "cmd_handler.h"
#include "motor.h"
#include "navigation.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"
#include "esp_task_wdt.h"



/// @ { Can not call RTOS delay because it will context switch
#define MS_TO_US(ms)   (ms            * 1000UL)
#define SEC_TO_MS(sec) (MS_TO_US(sec) * 1000UL)
#define DELAY_SEC_WITHOUT_CONTEXT_SWITCH(sec) (DELAY_US(SEC_TO_MS(sec)))
/// @ }

static void execute_self_test_routine(void)
{
    DISABLE_EXTERNAL_COMMANDS();
    {
        ESP_LOGI("SELF_TEST", "Part 1: Testing servo...");
        {
            for (uint8_t duty = 0; duty < 180; duty++)
            {
                motor_move(motor_servo, motor_dir_a_forward, (float)duty);
                DELAY_US(MS_TO_US(10));
            }

            for (int16_t duty = 180; duty >= 0; duty--)
            {
                motor_move(motor_servo, motor_dir_a_forward, (float)duty);
                DELAY_US(MS_TO_US(10));
            }

            motor_stop(motor_servo);
        }

        ESP_LOGI("SELF_TEST", "Part 2: Testing wheels...");
        {
            ESP_LOGI("SELF_TEST", "\tTesting forward...");
            motor_move(motor_wheels, motor_dir_both_forward, 40.0f);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(2UL);
            motor_stop(motor_wheels);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1UL);

            ESP_LOGI("SELF_TEST", "\tTesting backward...");
            motor_move(motor_wheels, motor_dir_both_backward, 40.0f);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(2UL);
            motor_stop(motor_wheels);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1UL);
            
            ESP_LOGI("SELF_TEST", "\tTesting left pivot...");
            motor_move(motor_wheels, motor_dir_pivot_left, 40.0f);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(2UL);
            motor_stop(motor_wheels);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1UL);

            ESP_LOGI("SELF_TEST", "\tTesting right pivot...");
            motor_move(motor_wheels, motor_dir_pivot_right, 40.0f);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(2UL);
            motor_stop(motor_wheels);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1UL);

            ESP_LOGI("SELF_TEST", "\tTesting delivery forward...");
            motor_move(motor_delivery, motor_dir_delivery_forward, 100.0f);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(2UL);
            motor_stop(motor_delivery);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1UL);

            //// @TODO : Does not work for some reason, double check GPIOs
            ESP_LOGI("SELF_TEST", "\tTesting delivery backward...");
            motor_move(motor_delivery, motor_dir_delivery_backward, 100.0f);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(2UL);
            motor_stop(motor_delivery);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1UL);
        }
    }
    ENABLE_EXTERNAL_COMMANDS();
}

void task_self_test(task_param_T params)
{
    DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1UL);

    ESP_LOGI("task_self_test", "Task initialized and starting...");

    // Main Loop
    while(1)
    {
        if (xSemaphoreTake(SelfTestSemaphore, MAX_DELAY))
        {
            execute_self_test_routine();
        }
    }
}