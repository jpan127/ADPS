#include "tasks.h"
// Project libraries
#include "cmd_handler.h"
#include "gpio.h"
#include "motor.h"
#include "navigation.h"
#include "packet_structure.h"



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
            .pwm       = { .unit = MCPWM_UNIT_0, .timer = MCPWM_TIMER_0 },
            .frequency = 1000,
        },
    };

    if (!motor_init(motor_wheels, &config))
    {
        LOG_ERROR("init_task_navigation", "Motor wheels were not properly initialized.");
        initialized = false;
    }
}

static void service_navigation_command(void)
{
    // Get the last received navigation command packet
    const command_packet_S * const packet = cmd_handler_get_last_navigation_packet();

    // Get the string version of the opcode for logging
    const char * opcode_string = opcode_to_string(packet->opcode);

    // Service command
    switch (packet->opcode)
    {
        case packet_opcode_stop          : { motor_stop(motor_wheels);                                                                      break; }
        case packet_opcode_move_forward  : { motor_move(motor_wheels, motor_dir_both_forward, packet->command[0]);                          break; }
        case packet_opcode_move_backward : { motor_move(motor_wheels, motor_dir_both_backward , packet->command[0]);                        break; }
        case packet_opcode_move_left     : { motor_move(motor_wheels, motor_dir_left_forward, packet->command[0]);                          break; }
        case packet_opcode_move_right    : { motor_move(motor_wheels, motor_dir_right_forward, packet->command[0]);                         break; }
        case packet_opcode_incr_forward  : { motor_adjust_duty(motor_wheels, motor_dir_both_forward, packet->command[0], duty_increment);   break; }
        case packet_opcode_incr_backward : { motor_adjust_duty(motor_wheels, motor_dir_both_backward , packet->command[0], duty_increment); break; }
        case packet_opcode_incr_left     : { motor_adjust_duty(motor_wheels, motor_dir_left, packet->command[0], duty_increment);           break; }
        case packet_opcode_incr_right    : { motor_adjust_duty(motor_wheels, motor_dir_right, packet->command[0], duty_increment);          break; }
        case packet_opcode_decr_forward  : { motor_adjust_duty(motor_wheels, motor_dir_both_forward, packet->command[0], duty_decrement);   break; }
        case packet_opcode_decr_backward : { motor_adjust_duty(motor_wheels, motor_dir_both_backward , packet->command[0], duty_decrement); break; }
        case packet_opcode_decr_left     : { motor_adjust_duty(motor_wheels, motor_dir_left, packet->command[0], duty_decrement);           break; }
        case packet_opcode_decr_right    : { motor_adjust_duty(motor_wheels, motor_dir_right, packet->command[0], duty_decrement);          break; }
        case packet_opcode_pivot_left    : { navigation_pivot_left();                                                                       break; }
        case packet_opcode_pivot_right   : { navigation_pivot_right();                                                                      break; }
        default                          : { opcode_string = NULL;                                                                          break; }
    }

    // Log command
    if (NULL == opcode_string)
    {
        ESP_LOGE("service_navigation_command", "Undefined opcode found: %d", packet->opcode);
    }
    else
    {
        ESP_LOGI("service_navigation_command", "%s", opcode_string);
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
        if (xSemaphoreTake(NavigationSemaphore, MAX_DELAY))
        {
            service_navigation_command();
        }
    }
}
