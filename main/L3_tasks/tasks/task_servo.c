#include "tasks.h"

#include "motor.h"
#include "gpio.h"



static const motor_E motor = motor_servo;

void init_task_servo(void)
{
    motor_config_S config =
    {
        .en_a      = GPIO_NOT_USING,
        .en_b      = GPIO_NOT_USING,
        .pwm_a     = gpio_get_pin_number(gpio_servo_pwm),
        .pwm_b     = GPIO_NOT_USING,
        .pwm_unit  = MCPWM_UNIT_1,
        .timer     = MCPWM_TIMER_0,
        .frequency = 50,
    };

    if (!motor_init(motor, &config))
    {
        ESP_LOGE("init_task_servo", "Motor servo was not properly initialized.");
    }
}

void task_servo(task_param_T params)
{
    // Main loop
    while(1)
    {

    }
}
