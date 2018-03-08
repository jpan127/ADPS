#include "tasks.h"

#include "motor.h"
#include "gpio.h"



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

void task_servo(task_param_T params)
{
    if (!initialized)
    {
        vTaskSuspend(NULL);
    }

    // Main loop
    while(1)
    {
        for (float duty = 0; duty < 180; duty++)
        {
            motor_move(motor, motor_dir_a_forward, duty);
            DELAY_MS(20);
        }

        motor_stop(motor);
        DELAY_MS(2000);

        for (float duty = 180; duty >= 0; duty--)
        {
            motor_move(motor, motor_dir_a_forward, duty);
            DELAY_MS(20);
        }

        motor_stop(motor);
        DELAY_MS(2000);
    }
}
