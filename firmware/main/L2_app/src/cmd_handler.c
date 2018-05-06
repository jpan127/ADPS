#include "cmd_handler.h"

#include "motor.h"



/// Flag for enabling manual controls or not
static bool manual_mode_enabled = false;

/// Flag for currently firmware controlling navigation and ignoring external commands
static bool override_in_progress = false;

void cmd_handler_set_override(const bool on)
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
                navigation_deliver_package();
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
            case packet_opcode_change_state:
            {
                if (packet->command[0] < navigation_state_last_invalid)
                {
                    navigation_set_state((navigation_state_E)(packet->command[0]));
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
