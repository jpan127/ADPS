#include "tasks.h"

#include "motor.h"



static bool initialized = true;
static const motor_E motor = motor_delivery;

void init_task_delivery(void)
{
    const motor_config_S config =
    {
        .dir_a  = gpio_delivery_en_a,
        .dir_b  = gpio_delivery_en_b,
        .config = 
        {
            .pwm_a = gpio_delivery_pwm,
            .pwm_b = GPIO_NOT_USING,
            .pwm   =  
            { 
                .unit = MCPWM_UNIT_0, 
                .timer = MCPWM_TIMER_1, 
            },
            .frequency = 1000,
        },
    };

    if (!motor_init(motor, &config))
    {
        initialized = false;
    }
}

void task_delivery(task_param_T params)
{
    if (!initialized)
    {
        ESP_LOGE("task_delivery", "Motor delivery was not properly initialized, suspending task...");
        vTaskSuspend(NULL);
    }

    static const uint16_t time_to_drop_package_ms = 7000;

    // Main loop
    while (1)
    {
#if TESTING
        motor_move(motor_delivery, motor_dir_delivery_forward, 100.0f);
        DELAY_MS(time_to_drop_package_ms);
        motor_stop(motor_delivery);
        DELAY_MS(3000);
#else

#endif
    }
}
