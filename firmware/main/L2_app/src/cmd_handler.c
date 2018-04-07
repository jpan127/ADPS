#include "cmd_handler.h"

#include "motor.h"



/// Flag for enabling manual controls or not
static bool manual_mode_enabled = false;

void cmd_handler_service_command(const command_packet_S * const packet)
{
    switch (packet->opcode)
    {
        case packet_opcode_manual_mode   : { ESP_LOGI("", "packet_opcode_manual_mode  "); manual_mode_enabled = (packet->command[0] == 0) ? (false) : (true);                             break; }
        case packet_opcode_stop          : { ESP_LOGI("", "packet_opcode_stop         ");        motor_stop(motor_wheels);                                                                break; }

        case packet_opcode_move_forward  : { ESP_LOGI("", "packet_opcode_move_forward ");        motor_move(motor_wheels , motor_dir_both_forward  , packet->command[0]);                 break; }
        case packet_opcode_move_backward : { ESP_LOGI("", "packet_opcode_move_backward");        motor_move(motor_wheels , motor_dir_both_backward , packet->command[0]);                 break; }
        case packet_opcode_move_left     : { ESP_LOGI("", "packet_opcode_move_left    ");        motor_move(motor_wheels , motor_dir_a_forward     , packet->command[0]);                 break; }
        case packet_opcode_move_right    : { ESP_LOGI("", "packet_opcode_move_right   ");        motor_move(motor_wheels , motor_dir_a_backward    , packet->command[0]);                 break; }
        
        case packet_opcode_incr_forward  : { ESP_LOGI("", "packet_opcode_incr_forward "); motor_adjust_duty(motor_wheels , motor_dir_both_forward  , packet->command[0], duty_increment); break; }
        case packet_opcode_incr_backward : { ESP_LOGI("", "packet_opcode_incr_backward"); motor_adjust_duty(motor_wheels , motor_dir_both_backward , packet->command[0], duty_increment); break; }
        case packet_opcode_incr_left     : { ESP_LOGI("", "packet_opcode_incr_left    "); motor_adjust_duty(motor_wheels , motor_dir_a_forward     , packet->command[0], duty_increment); break; }
        case packet_opcode_incr_right    : { ESP_LOGI("", "packet_opcode_incr_right   "); motor_adjust_duty(motor_wheels , motor_dir_b_forward     , packet->command[0], duty_increment); break; }
        
        case packet_opcode_decr_forward  : { ESP_LOGI("", "packet_opcode_decr_forward "); motor_adjust_duty(motor_wheels , motor_dir_both_forward  , packet->command[0], duty_decrement); break; }
        case packet_opcode_decr_backward : { ESP_LOGI("", "packet_opcode_decr_backward"); motor_adjust_duty(motor_wheels , motor_dir_both_backward , packet->command[0], duty_decrement); break; }
        case packet_opcode_decr_left     : { ESP_LOGI("", "packet_opcode_decr_left    "); motor_adjust_duty(motor_wheels , motor_dir_a_backward    , packet->command[0], duty_decrement); break; }
        case packet_opcode_decr_right    : { ESP_LOGI("", "packet_opcode_decr_right   "); motor_adjust_duty(motor_wheels , motor_dir_b_backward    , packet->command[0], duty_decrement); break; }
        
        case packet_opcode_servo_duty    : { ESP_LOGI("", "packet_opcode_servo_duty   ");        motor_move(motor_servo  , motor_dir_a_forward     , packet->command[0]);                 break; }
        default                          : { LOG_ERROR("Undefined opcode found: %d", packet->opcode);                                                                                     break; }
    }
}
