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
            motor_move(motor_servo, motor_dir_left_forward, 0.0f);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1);
            for (uint8_t duty = 0; duty < 180; duty++)
            {
                motor_move(motor_servo, motor_dir_left_forward, (float)duty);
                DELAY_US(MS_TO_US(5));
            }

            for (int16_t duty = 180; duty >= 0; duty--)
            {
                motor_move(motor_servo, motor_dir_left_forward, (float)duty);
                DELAY_US(MS_TO_US(5));
            }

            motor_stop(motor_servo);
            DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1);
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

    ESP_LOGI("task_self_test", "Task initialized and starting...");
    DELAY_SEC_WITHOUT_CONTEXT_SWITCH(1UL);

    // Main Loop
    while (1)
    {
        if (xSemaphoreTake(SelfTestSemaphore, MAX_DELAY))
        {
            set_suspend_state(true);
            execute_self_test_routine();
            set_suspend_state(false);
        }
    }
}
