#include "tasks.h"
// Project libraries
#include "gpio.h"
#include "motor.h"



/// Enumeration to specify how to pivot
typedef enum
{
    pivot_direction_left_90_degrees,
    pivot_direction_right_90_degrees,
} pivot_direction_E;

static const motor_E motor = motor_wheels;
static bool initialized = true;

static void pivot_90_degrees(const pivot_direction_E pivot_direction)
{
    switch (pivot_direction)
    {
        case pivot_direction_left_90_degrees:
            motor_move(motor_wheels, motor_dir_a_forward, 50);
            motor_move(motor_wheels, motor_dir_b_forward,  0);
            break;
        case pivot_direction_right_90_degrees:
            motor_move(motor_wheels, motor_dir_a_forward,  0);
            motor_move(motor_wheels, motor_dir_b_forward, 50);
            break;
        default:
            break;
    }
}

void init_task_navigation(void)
{
    const motor_config_S config =
    {
        .dir_a  = gpio_wheels_en_a,
        .dir_b  = gpio_wheels_en_b,
        .config = 
        {
            .pwm_a     = gpio_wheels_pwm_a,
            .pwm_b     = gpio_wheels_pwm_b,
            .pwm       =  { .unit = MCPWM_UNIT_0, .timer = MCPWM_TIMER_0, },
            .frequency = 1000,
        },
    };

    if (!motor_init(motor, &config))
    {
        LOG_ERROR("init_task_navigation", "Motor wheels were not properly initialized.");
        initialized = false;
    }
}

void task_navigation(task_param_T params)
{
    static const uint32_t delay_between_duty_changes_ms = 200;

    static const uint32_t delay_to_pivot_90_degrees = 3100;
    
    if (!initialized)
    {
        vTaskSuspend(NULL);
    }

    // Main loop
    while (1)
    {
        // for (float duty=0; duty<100; duty++)
        // {
        //     motor_move(motor, motor_dir_both_forward, duty);
        //     DELAY_MS(delay_between_duty_changes_ms);
        // }

        // motor_stop(motor);
        // DELAY_MS(2000);
        
        // for (float duty=0; duty<100; duty++)
        // {
        //     motor_move(motor, motor_dir_both_backward, duty);
        //     DELAY_MS(delay_between_duty_changes_ms);
        // }

        // motor_stop(motor);
        // DELAY_MS(2000);

        // for (float duty=0; duty<100; duty++)
        // {
        //     motor_move(motor, motor_dir_b_forward, duty);
        //     DELAY_MS(delay_between_duty_changes_ms);
        // }

        // motor_stop(motor);
        // DELAY_MS(2000);

        // for (float duty=0; duty<100; duty++)
        // {
        //     motor_move(motor, motor_dir_a_forward, duty);
        //     DELAY_MS(delay_between_duty_changes_ms);
        // }

        // motor_stop(motor);
        // DELAY_MS(2000);

        // pivot_90_degrees(pivot_direction_left_90_degrees);
        // DELAY_MS(3100);

        
        // pivot_90_degrees(pivot_direction_right_90_degrees);
        // DELAY_MS(3100);
    }
}
