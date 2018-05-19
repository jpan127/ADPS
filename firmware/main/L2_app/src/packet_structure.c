#include "packet_structure.h"

const char * opcode_to_string(const packet_opcode_E opcode)
{
    switch (opcode)
    {
        case packet_opcode_move_forward: { return "packet_opcode_move_forward"; break; }
        case packet_opcode_move_backward: { return "packet_opcode_move_backward"; break; }
        case packet_opcode_move_left: { return "packet_opcode_move_left"; break; }
        case packet_opcode_move_right: { return "packet_opcode_move_right"; break; }
        case packet_opcode_incr_forward: { return "packet_opcode_incr_forward"; break; }
        case packet_opcode_incr_backward: { return "packet_opcode_incr_backward"; break; }
        case packet_opcode_incr_left: { return "packet_opcode_incr_left"; break; }
        case packet_opcode_incr_right: { return "packet_opcode_incr_right"; break; }
        case packet_opcode_decr_forward: { return "packet_opcode_decr_forward"; break; }
        case packet_opcode_decr_backward: { return "packet_opcode_decr_backward"; break; }
        case packet_opcode_decr_left: { return "packet_opcode_decr_left"; break; }
        case packet_opcode_decr_right: { return "packet_opcode_decr_right"; break; }
        case packet_opcode_servo_duty: { return "packet_opcode_servo_duty"; break; }
        case packet_opcode_manual_mode: { return "packet_opcode_manual_mode"; break; }
        case packet_opcode_stop: { return "packet_opcode_stop"; break; }
        case packet_opcode_deliver: { return "packet_opcode_deliver"; break; }
        case packet_opcode_pivot_left: { return "packet_opcode_pivot_left"; break; }
        case packet_opcode_pivot_right: { return "packet_opcode_pivot_right"; break; }
        case packet_opcode_change_state: { return "packet_opcode_change_state"; break; }
        case packet_opcode_execute_self_test: { return "packet_opcode_execute_self_test"; break; }
        case packet_opcode_set_suspend: { return "packet_opcode_set_suspend"; break; }
        default: { return "NULL??????"; break; }
    }
};