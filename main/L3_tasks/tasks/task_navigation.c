#include "tasks.h"
// Project libraries
#include "gpio.h"
#include "motor.h"


static const motor_E motor = motor_wheels;

void init_task_navigation(void)
{
    motor_config_S config =
    {
        .en_a      = gpio_get_pin_number(gpio_wheels_en_a),
        .en_b      = gpio_get_pin_number(gpio_wheels_en_b),
        .pwm_a     = gpio_get_pin_number(gpio_wheels_pwm_a),
        .pwm_b     = gpio_get_pin_number(gpio_wheels_pwm_b),
        .pwm_unit  = MCPWM_UNIT_0,
        .timer     = MCPWM_TIMER_0,
        .frequency = 1000,
    };

    if (!motor_init(motor, &config))
    {
        ESP_LOGE("init_task_navigation", "Motor wheels were not properly initialized.");
    }
}

void task_navigation(task_param_T params)
{
    static const uint32_t delay_between_duty_changes_ms = 200;

    // Main loop
    while (1)
    {
        for (float duty=0; duty<100; duty++)
        {
            motor_move(motor, motor_dir_both_forward, duty);
            DELAY_MS(delay_between_duty_changes_ms);
        }

        motor_stop(motor);
        DELAY_MS(2000);
        
        for (float duty=0; duty<100; duty++)
        {
            motor_move(motor, motor_dir_both_backward, duty);
            DELAY_MS(delay_between_duty_changes_ms);
        }

        motor_stop(motor);
        DELAY_MS(2000);
    }
}
