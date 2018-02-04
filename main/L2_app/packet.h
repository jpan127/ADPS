#pragma once
// Project libraries
#include "common.h"



/**
 *  @module      : packet
 *  @description : Handles parsing command packets and sending diagnostic packets
 */

/// Max size of diagnostic packet payload
#define MAX_PACKET_SIZE (128)

extern QueueHandle_t MessageTxQueue;
extern QueueHandle_t MessageRxQueue;

/// Denotes the type of the packet
typedef enum
{
    PACKET_TYPE_INFO          = 0,  ///< System starting/finishing/initializing something, x bytes were sent
    PACKET_TYPE_ERROR         = 1,  ///< Something failed
    PACKET_TYPE_STATUS        = 2,  ///< Something successful, something complete
    PACKET_TYPE_LOG           = 3,  ///< Data logging
    PACKET_TYPE_COMMAND_READ  = 4,  ///< Get commands
    PACKET_TYPE_COMMAND_WRITE = 5,  ///< Set commands

    log_type_client = 0x80,
    log_type_server = 0x81,
    log_type_packet = 0x82,
    log_type_motor  = 0x83,
    log_type_wifi   = 0x84,

    PACKET_TYPE_LAST_INVALID,
} packet_type_E;

/// Denotes the opcode for command packets
typedef enum
{
    command_move_forward   = 0,
    command_move_backward  = 1,
    command_move_left      = 2,
    command_move_right     = 3,

    command_incr_forward   = 4,
    command_incr_backward  = 5,
    command_incr_left      = 6,
    command_incr_right     = 7,

    command_decr_forward   = 8,
    command_decr_backward  = 9,
    command_decr_left      = 10,
    command_decr_right     = 11,
} packet_opcode_E;

/// Denotes the current state of the parser
typedef enum
{
    PARSER_IDLE,
    PARSER_IN_PROGRESS,
    PARSER_COMPLETE,
    PARSER_ERROR
} parser_status_E;

/// Diagnostic Packet structure
typedef struct
{
    uint8_t type;                       ///< Type of packet
    uint8_t length;                     ///< Size of payload in bytes
    uint8_t payload[MAX_PACKET_SIZE];   ///< Data

} __attribute__((packed)) diagnostic_packet_S;

/// Command Packet structure
typedef struct
{
    uint8_t type;                       ///< Type of packet
    uint8_t opcode;                     ///< Opcode of command

    union
    {
        uint8_t  bytes[2];
        uint16_t half_word;
    } command;

} __attribute__((packed)) command_packet_S;

/// Packet logging struct
typedef struct
{
    uint32_t rx_packets;
    uint32_t tx_packets;
    uint32_t packet_errors;
    uint32_t packet_counts[PACKET_TYPE_LAST_INVALID];
} packet_logs_S;

/**
 *  State machine to parse a command packet
 *  @param byte   : The next byte to be parsed
 *  @param packet : Pointer to the packet to be modified
 *  @returns      : Status of parser state machine
 */
parser_status_E command_packet_parser(uint8_t byte, command_packet_S *packet);

/**
 *  Printf-style printing a formatted string to the server
 *      1. log_to_server
 *      2. log_vsprintf
 *      3. create_diagnostic_packet
 *      4. msg_enqueue_no_timeout
 * @param type    : The type of the packet
 * @param message : The string format 
 */
void log_to_server(packet_type_E type, const char *message, ...);

/**
 *  Inline wrappers for logging log data
 *      - uint32_t
 *      - float
 *      - const char *
 */
inline void log_data(const char *message, uint8_t category, uint32_t *data)          { if (data) LOG_LOG(message, category, *data); }
inline void log_data_float(const char *message, uint8_t category, float *data)       { if (data) LOG_LOG(message, category, *data); }
inline void log_data_string(const char *message, uint8_t category, const char *data) { if (data) LOG_LOG(message, category,  data); }

/// Returns a pointer to the packet logging struct
packet_logs_S * packet_get_logs(void);
