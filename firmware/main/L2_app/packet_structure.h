#pragma once
// Project libraries
#include "common.h"



/// Max size of diagnostic packet payload
#define MAX_PACKET_SIZE (128)

/// Denotes the type of the packet
typedef enum
{
    packet_type_info = 0,
    packet_type_error = 1,
    packet_type_status = 2,
    packet_type_log = 3,
    packet_type_log_client = 10,
    packet_type_log_server = 11,
    packet_type_log_packet = 12,
    packet_type_log_motor = 13,
    packet_type_log_wifi = 14,
    packet_type_log_wmark = 15,
    packet_type_log_infrared = 16,

    packet_type_last_invalid,
} packet_type_E;

/// Denotes the opcode for command packets
typedef enum
{
    packet_opcode_move_forward = 0,
    packet_opcode_move_backward = 1,
    packet_opcode_move_left = 2,
    packet_opcode_move_right = 3,
    packet_opcode_incr_forward = 4,
    packet_opcode_incr_backward = 5,
    packet_opcode_incr_left = 6,
    packet_opcode_incr_right = 7,
    packet_opcode_decr_forward = 8,
    packet_opcode_decr_backward = 9,
    packet_opcode_decr_left = 10,
    packet_opcode_decr_right = 11,
    packet_opcode_servo_duty = 12,
    packet_opcode_manual_mode = 13,
    packet_opcode_stop = 14,
    packet_opcode_deliver = 15,
    packet_opcode_pivot_left = 16,
    packet_opcode_pivot_right = 17,
    packet_opcode_change_state = 18,

    packet_opcode_last_invalid,
} packet_opcode_E;

/// Diagnostic Packet structure
typedef struct
{
    uint8_t type;
    uint8_t length;
    uint8_t payload[128];

} __attribute__((packed)) diagnostic_packet_S;

/// Command Packet structure
typedef struct
{
    uint8_t opcode;
    uint8_t command[2];

} __attribute__((packed)) command_packet_S;
