#include "packet.h"
// Standard libraries
#include <string.h>
#include <stdio.h>


/// Current state/status of the parser
typedef enum
{
    parser_state_opcode = 0,
    parser_state_command1,
    parser_state_command2,
} parser_state_E;

/// Logging struct for packet
static packet_logs_S logs = { 0 };

/// Packet waits a maximum of this many millseconds
static const TickType_t max_packet_wait = 100;

static uint8_t packet_strlen_buffer(const char * buffer, const size_t size)
{
    uint8_t length = 0;

    for (length = 0; length < size; length++)
    {
        if (buffer[length] == 0)
        {
            break;
        }
    }

    return length;
}

static void packet_vsnprintf(const packet_type_E type, const char * message, va_list arg_list)
{
    // Buffer for post-formatted message
    char buffer[MAX_PACKET_SIZE + 2] = { 0 };

    // Print warning if larger than the max packet size
    if (strlen(message) > MAX_PACKET_SIZE + 2)
    {
        ESP_LOGE("packet_vsnprintf", "Message over max buffer size.\n");
    }

    // Prints formatted message to buffer with null-termination
    vsnprintf(buffer+2, sizeof(diagnostic_packet_S) - 2, message, arg_list);

    buffer[0] = (uint8_t)type;
    buffer[1] = packet_strlen_buffer(buffer+2, MAX_PACKET_SIZE);

    // Convert buffer to diagnostic packet
    diagnostic_packet_S * const packet = (diagnostic_packet_S *)(&buffer);

    // Send to TX queue
    if (!xQueueSend(MessageTxQueue, packet, max_packet_wait))
    {
        ESP_LOGE("packet_vsnprintf", "MessageTxQueue full, failed to send to queue");
        logs.packets_dropped++;
    }

    logs.tx_packets++;
    logs.packet_counts[type]++;
}

void log_to_server(const packet_type_E type, const char * message, ...)
{
    va_list arg_list;
    va_start(arg_list, message);
    {
        packet_vsnprintf(type, message, arg_list);
    }
    va_end(arg_list);
}

parser_status_E command_packet_parser(const uint8_t * const buffer, command_packet_S * const packet)
{
    parser_status_E status = parser_status_complete;

    packet->opcode     = buffer[0];
    packet->command[0] = buffer[1];
    packet->command[1] = buffer[2];

    // Sanity check
    if (packet->opcode >= packet_opcode_last_invalid)
    {
        status = parser_status_error;
    }

    return status;
}

packet_logs_S * packet_get_logs(void)
{
    return &logs;
}
