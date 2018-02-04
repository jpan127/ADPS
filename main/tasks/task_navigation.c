#include "tasks.h"
// Project libraries
#include "gpio.h"
#include "motor.h"


static motor_config_S config = { 0 };
static const motor_E motor   = MOTOR_WHEELS;

void init_task_navigation(void)
{
    config.en_a     = gpio_get_pin_number(gpio_wheels_en_a);
    config.en_b     = gpio_get_pin_number(gpio_wheels_en_b);
    config.pwm_a    = gpio_get_pin_number(gpio_wheels_pwm_a);
    config.pwm_b    = gpio_get_pin_number(gpio_wheels_pwm_b);
    config.pwm_unit = MCPWM_UNIT_0;
    config.timer    = MCPWM_TIMER_0;

    if (!motor_init(motor, &config))
    {
        ESP_LOGE("init_task_navigation", "Motor wheels were not properly initialized.");
    }
}

void task_navigation(void *p)
{
    const uint32_t delay_between_duty_changes_ms = 200;

    // Main loop
    while (1)
    {
        for (float duty=0; duty<100; duty++)
        {
            motor_move(motor, MOTOR_DIRECTION_FORWARD, duty);
            DELAY_MS(delay_between_duty_changes_ms);
        }

        motor_stop(motor);
        DELAY_MS(2000);
        
        for (float duty=0; duty<100; duty++)
        {
            motor_move(motor, MOTOR_DIRECTION_BACKWARD, duty);
            DELAY_MS(delay_between_duty_changes_ms);
        }

        motor_stop(motor);
        DELAY_MS(2000);
    }
}