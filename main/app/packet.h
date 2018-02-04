#pragma once
// Project libraries
#include "common.h"


/**
 *  @module      : packet
 *  @description : Handles parsing command packets and sending diagnostic packets
 */


// Max size of diagnostic packet payload
#define MAX_PACKET_SIZE (128)

extern QueueHandle_t MessageTxQueue;

// Denotes the type of the packet
typedef enum
{
    PACKET_TYPE_INFO          = 0,  // System starting/finishing/initializing something, x bytes were sent
    PACKET_TYPE_ERROR         = 1,  // Something failed
    PACKET_TYPE_STATUS        = 2,  // Something successful, something complete
    PACKET_TYPE_LOG           = 3,  // Data logging
    PACKET_TYPE_COMMAND_READ  = 4,  // Get commands
    PACKET_TYPE_COMMAND_WRITE = 5,  // Set commands
} packet_type_E;

// Denotes the opcode for command packets
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

// Denotes the current state of the parser
typedef enum
{
    PARSER_IDLE,
    PARSER_IN_PROGRESS,
    PARSER_COMPLETE,
    PARSER_ERROR
} parser_status_E;

// Diagnostic Packet structure
typedef struct
{
    uint8_t type;                       ///< Type of packet
    uint8_t length;                     ///< Size of payload in bytes
    uint8_t payload[MAX_PACKET_SIZE];   ///< Data

} __attribute__((packed)) diagnostic_packet_S;

// Command Packet structure
typedef struct
{
    uint8_t type;   // Type of packet
    uint8_t opcode; // Opcode of command

    union
    {
        uint8_t  bytes[2];
        uint16_t half_word;
    } command;

} __attribute__((packed)) command_packet_S;

// @description  : State machine to parse a command packet
// @param byte   : The next byte to be parsed
// @param packet : Pointer to the packet to be modified
// @returns      : Status of parser state machine
parser_status_E command_packet_parser(uint8_t byte, command_packet_S *packet);

// @description   : Printf-style printing a formatted string to the server
//                  1. log_to_server
//                  2. log_vsprintf
//                  3. create_diagnostic_packet
//                  4. msg_enqueue_no_timeout
// @param type    : The type of the packet
// @param message : The string format 
void log_to_server(packet_type_E type, const char *message, ...);

/**
 *  Logging data must have this format : { category, sub_category, data1, data2 }
 *
 */
void log_data(const char *category, uint8_t sub_category, uint32_t data1, uint32_t data2);