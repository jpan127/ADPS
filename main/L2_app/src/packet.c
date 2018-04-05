#include "packet.h"
// Standard libraries
#include <string.h>
#include <stdio.h>


/// Current state/status of the parser
typedef enum
{
    parser_state_type     = 0,
    parser_state_opcode   = 1,
    parser_state_command1 = 2,
    parser_state_command2 = 3,
} parser_state_E;

/// Logging struct for packet
static packet_logs_S logs =
{
    .rx_packets    = 0,
    .tx_packets    = 0,
    .packet_errors = 0,
    .packet_counts = { 0 },
};

static void log_vsnprintf(packet_type_E type, const char *message, va_list arg_list)
{
    // Buffer for post-formatted message
    char buffer[MAX_PACKET_SIZE + 2] = { 0 };

    // Print warning if larger than the max packet size
    if (strlen(message) > MAX_PACKET_SIZE + 2)
    {
        ESP_LOGE("log_vsnprintf", "Message over max buffer size.\n");
    }

    // Prints formatted message to buffer with null-termination
    vsnprintf(buffer+2, sizeof(diagnostic_packet_S), message, arg_list);

#if TESTING
    switch (type)
    {
        case packet_type_error : ESP_LOGE("", "%s", buffer+2); break;   ///< Errors go to LOGE
        default                : ESP_LOGI("", "%s", buffer+2); break;   ///< All else goes to LOGI
    }
#else
    buffer[0] = (uint8_t)type;
    buffer[1] = strlen(buffer) - 2;

    // Convert buffer to diagnostic packet
    diagnostic_packet_S * const packet = (diagnostic_packet_S *)(&buffer);

    // Send to TX queue
    xQueueSend(MessageTxQueue, packet, MAX_DELAY);
#endif

    logs.tx_packets++;
    logs.packet_counts[type]++;
}

void log_to_server(packet_type_E type, const char *message, ...)
{
    va_list arg_list;
    va_start(arg_list, message);
    {
        log_vsnprintf(type, message, arg_list);
    }
    va_end(arg_list);
}

parser_status_E command_packet_parser(uint8_t byte, command_packet_S *packet)
{
    static parser_state_E state = parser_state_type;

    switch (state)
    {
        case parser_state_type:

            packet->type = byte;
            state = parser_state_opcode;
            return parser_status_in_progress;
        
        case parser_state_opcode:
        
            packet->opcode = byte;
            state = parser_state_command1;
            return parser_status_in_progress;
        
        case parser_state_command1:
        
            packet->command.bytes[0] = byte;
            state = parser_state_command2;
            return parser_status_in_progress;
        
        case parser_state_command2:
        
            packet->command.bytes[1] = byte;
            state = parser_state_type;
            return parser_status_complete;
        
        default:
        
            ESP_LOGE("command_packet_parser", "Reached impossible state: %d!\n", state);
            state = parser_state_type;
            return parser_status_error;
    }
}

packet_logs_S * packet_get_logs(void)
{
    return &logs;
}