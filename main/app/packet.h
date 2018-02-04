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
    COMMAND_SET_FORWARD   = 0,    /// @TODO : This should be in a universal JSON
    COMMAND_SET_BACKWARD  = 1,
    COMMAND_SET_LEFT      = 2,
    COMMAND_SET_RIGHT     = 3,
    COMMAND_GET_FORWARD   = 4,
    COMMAND_GET_BACKWARD  = 5,
    COMMAND_GET_LEFT      = 6,
    COMMAND_GET_RIGHT     = 7,
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

/// Struct for logging packet information
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

packet_logs_S * packet_get_logs(void);