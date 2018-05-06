#include "cmd_handler.h"

#include "motor.h"



/// Enumeration to specify how to pivot
typedef enum
{
    pivot_direction_left_90_degrees,
    pivot_direction_right_90_degrees,
} pivot_direction_E;

/// Flag for enabling manual controls or not
static bool manual_mode_enabled = false;

static bool override_in_progress = false;

static navigation_state_E current_navigation_state = navigation_state_navigating_sidewalk;

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

void cmd_handler_set_override(bool on)
{
    override_in_progress = on;
}

void cmd_handler_service_command(const command_packet_S * const packet)
{
    // Ignore commands when override is in progress
    if (!override_in_progress)
    {
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
                static const float right_motor_negative_offset = 5.0f;
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
            case packet_opcode_servo_duty:
            {
                ESP_LOGI("COMMAND", "packet_opcode_servo_duty");
                motor_move(motor_servo, motor_dir_a_forward, packet->command[0]);
                break;
            }
            case packet_opcode_manual_mode:
            { 
                ESP_LOGI("COMMAND", "packet_opcode_manual_mode"); 
                manual_mode_enabled = (packet->command[0] != 0);
                break;
            }
            case packet_opcode_deliver:
            {
                ESP_LOGI("COMMAND", "packet_opcode_deliver");
                deliver_package();
                break;
            }
            case packet_opcode_pivot_left:
            {
                ESP_LOGI("COMMAND", "packet_opcode_pivot_left");
                pivot_90_degrees(pivot_direction_left_90_degrees);
                break;
            }
            case packet_opcode_pivot_right:
            {
                ESP_LOGI("COMMAND", "packet_opcode_pivot_right");
                pivot_90_degrees(pivot_direction_right_90_degrees);
                break;
            }
            case packet_opcode_change_state:
            {
                if (packet->command[0] < navigation_state_last_invalid)
                {
                    current_navigation_state = packet->command[0];
                }
                break;
            }
            default:
            { 
                ESP_LOGE("COMMAND", "Undefined opcode found: %d", packet->opcode);
                break;
            }
        }
    }
}

void deliver_package(void)
{
    static const uint16_t time_to_drop_package_ms = 7000;
    motor_move(motor_delivery, motor_dir_delivery_forward, 100.0f);
    DELAY_MS(time_to_drop_package_ms);
    motor_stop(motor_delivery);    
}

void navigation_backup(float duty)
{
    motor_stop(motor_wheels);
    motor_move(motor_wheels, motor_dir_both_backward, duty);
}

navigation_state_E navigation_get_state(void)
{
    return current_navigation_state;
}