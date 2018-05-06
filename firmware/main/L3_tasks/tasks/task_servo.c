#include "tasks.h"

#include "motor.h"
#include "gpio.h"
#include "cmd_handler.h"


/**
 *  @TODO : This task may not be needed so can put the servo config in another function init_motors()
 *          Keep this task for testing maybe
 */
static const motor_E motor = motor_servo;
static bool initialized = true;

void init_task_servo(void)
{
    const motor_config_S config =
    {
        .dir_a  = GPIO_NOT_USING,
        .dir_b  = GPIO_NOT_USING,
        .config = 
        {
            .pwm_a     = gpio_servo_pwm,
            .pwm_b     = GPIO_NOT_USING,
            .pwm       = { .unit = MCPWM_UNIT_1, .timer = MCPWM_TIMER_0 },
            .frequency = 50,
        },
    };

    if (!motor_init(motor, &config))
    {
        LOG_ERROR("init_task_servo", "Motor servo was not properly initialized.");
        initialized = false;
    }
}

/**
 *  Boot up routine to test the servo turning to both sides of the spectrum
 */
static void servo_boot_up_routine(void)
{
    static const uint8_t delay_between_adjustments_ms = 10;

    ESP_LOGI("task_servo", "Initialized servo, running boot up routine...");

    for (uint8_t duty = 0; duty < 180; duty++)
    {
        motor_move(motor, motor_dir_a_forward, (float)duty);
        DELAY_MS(delay_between_adjustments_ms);
    }

    for (int16_t duty = 180; duty >= 0; duty--)
    {
        motor_move(motor, motor_dir_a_forward, (float)duty);
        DELAY_MS(delay_between_adjustments_ms);
    }

    motor_stop(motor);
}

void task_servo(task_param_T params)
{
    // Wait a second to make sure servo is properly initialized
    DELAY_MS(1000);

    if (!initialized)
    {
        ESP_LOGE("task_servo", "Did not successfully initialize servo, suspending task...");
        vTaskSuspend(NULL);
    }
    else
    {
        motor_move(motor, motor_dir_a_forward, 0);
        DELAY_MS(100);
        servo_boot_up_routine();
        ESP_LOGI("task_servo", "Task initialized and starting...");
    }

#if TESTING
    vTaskSuspend(NULL);
#else
    // Main loop
    while(1)
    {
        if (xSemaphoreTake(ServoSemaphore, MAX_DELAY))
        {
            const command_packet_S * const packet = cmd_handler_get_last_packet();
            motor_move(motor_servo, motor_dir_a_forward, packet->command[0]);
        }
    }
#endif
}
