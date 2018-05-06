#include "tasks.h"
// Project libraries
#include "gpio.h"
#include "motor.h"
#include "cmd_handler.h"
#include "navigation.h"



/// @TODO : Maybe remove this
static const motor_E motor = motor_wheels;

/// Flag to show init function was properly called or not
static bool initialized = true;

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

static void service_navigation_command(void)
{
    const command_packet_S * const packet = cmd_handler_get_last_packet();

    switch (packet->opcode)
    {
        case packet_opcode_stop:
        {
            ESP_LOGI("COMMAND", "packet_opcode_stop");
            motor_stop(motor_wheels);
            break;
        }
        case packet_opcode_move_forward:
        {
            ESP_LOGI("COMMAND", "packet_opcode_move_forward");
            motor_move(motor_wheels , motor_dir_both_forward, packet->command[0]);
            break;
        }
        case packet_opcode_move_backward:
        {
            ESP_LOGI("COMMAND", "packet_opcode_move_backward");
            motor_move(motor_wheels , motor_dir_both_backward , packet->command[0]);                 
            break;
        }
        case packet_opcode_move_left:
        {
            ESP_LOGI("COMMAND", "packet_opcode_move_left");
            motor_move(motor_wheels , motor_dir_a_forward, packet->command[0]);                 
            break;
        }
        case packet_opcode_move_right:
        {
            ESP_LOGI("COMMAND", "packet_opcode_move_right");
            motor_move(motor_wheels , motor_dir_b_forward, packet->command[0]);                 
            break;
        }
        case packet_opcode_incr_forward:
        {
            ESP_LOGI("COMMAND", "packet_opcode_incr_forward");
            motor_adjust_duty(motor_wheels , motor_dir_both_forward, packet->command[0], duty_increment); 
            break;
        }
        case packet_opcode_incr_backward:
        {
            ESP_LOGI("COMMAND", "packet_opcode_incr_backward");
            motor_adjust_duty(motor_wheels , motor_dir_both_backward , packet->command[0], duty_increment); 
            break;
        }
        case packet_opcode_incr_left:
        {
            ESP_LOGI("COMMAND", "packet_opcode_incr_left");
            motor_adjust_duty(motor_wheels , motor_dir_left, packet->command[0], duty_increment); 
            break;
        }
        case packet_opcode_incr_right:
        {
            ESP_LOGI("COMMAND", "packet_opcode_incr_right");
            motor_adjust_duty(motor_wheels , motor_dir_right, packet->command[0], duty_increment); 
            break;
        }
        case packet_opcode_decr_forward:
        {
            ESP_LOGI("COMMAND", "packet_opcode_decr_forward");
            motor_adjust_duty(motor_wheels , motor_dir_both_forward, packet->command[0], duty_decrement); 
            break;
        }
        case packet_opcode_decr_backward:
        {
            ESP_LOGI("COMMAND", "packet_opcode_decr_backward");
            motor_adjust_duty(motor_wheels , motor_dir_both_backward , packet->command[0], duty_decrement); 
            break;
        }
        case packet_opcode_decr_left:
        {
            ESP_LOGI("COMMAND", "packet_opcode_decr_left");
            motor_adjust_duty(motor_wheels , motor_dir_left, packet->command[0], duty_decrement); 
            break;
        }
        case packet_opcode_decr_right:
        {
            ESP_LOGI("COMMAND", "packet_opcode_decr_right");
            motor_adjust_duty(motor_wheels , motor_dir_right, packet->command[0], duty_decrement); 
            break;
        }
        case packet_opcode_pivot_left:
        {
            ESP_LOGI("COMMAND", "packet_opcode_pivot_left");
            navigation_pivot_left();
            break;
        }
        case packet_opcode_pivot_right:
        {
            ESP_LOGI("COMMAND", "packet_opcode_pivot_right");
            navigation_pivot_right();
            break;
        }
        default:
        { 
            ESP_LOGE("COMMAND", "Undefined opcode found: %d", packet->opcode);
            break;
        }
    }
}

void task_navigation(task_param_T params)
{
    if (!initialized)
    {
        ESP_LOGE("task_navigation", "Motor navigation was not properly initialized, suspending task...");
        vTaskSuspend(NULL);
    }
    else
    {
        ESP_LOGI("task_navigation", "Task initialized and starting...");
    }

    // Main loop
    while (1)
    {
#if 0
        static const uint32_t delay_between_duty_changes_ms = 200;
    
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

        for (float duty=0; duty<100; duty++)
        {
            motor_move(motor, motor_dir_b_forward, duty);
            DELAY_MS(delay_between_duty_changes_ms);
        }

        motor_stop(motor);
        DELAY_MS(2000);

        for (float duty=0; duty<100; duty++)
        {
            motor_move(motor, motor_dir_a_forward, duty);
            DELAY_MS(delay_between_duty_changes_ms);
        }

        motor_stop(motor);
        DELAY_MS(2000);

        pivot_90_degrees(pivot_direction_left_90_degrees);
        DELAY_MS(3100);

        
        pivot_90_degrees(pivot_direction_right_90_degrees);
        DELAY_MS(3100);
#else
        if (xSemaphoreTake(NavigationSemaphore, MAX_DELAY))
        {
            service_navigation_command();
        }
#endif
    }
}
