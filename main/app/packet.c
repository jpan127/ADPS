#include "packet.h"
// Standard libraries
#include <string.h>
#include <stdio.h>


/// Current state/status of the parser
typedef enum
{
    TYPE     = 0,
    OPCODE   = 1,
    COMMAND1 = 2,
    COMMAND2 = 3,
} parser_state_E;

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
        ESP_LOGE("[log_vsnprintf]", "Message over max buffer size.\n");
    }

    // Prints formatted message to buffer with null-termination
    vsnprintf(buffer+2, sizeof(diagnostic_packet_S), message, arg_list);

#if TERMINAL_TESTING
    ESP_LOGI("", "%s", buffer+2);
#else
    buffer[0] = (uint8_t)type;
    buffer[1] = strlen(buffer) - 2;

    // Convert buffer to diagnostic packet
    diagnostic_packet_S *packet = (diagnostic_packet_S *)(&buffer);

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
    log_vsnprintf(type, message, arg_list);
    va_end(arg_list);
}

parser_status_E command_packet_parser(uint8_t byte, command_packet_S *packet)
{
    static parser_state_E state = TYPE;

    switch (state)
    {
        case TYPE:
            packet->type = byte;
            state = OPCODE;
            return PARSER_IN_PROGRESS;
        case OPCODE:
            packet->opcode = byte;
            state = COMMAND1;
            return PARSER_IN_PROGRESS;
        case COMMAND1:
            packet->command.bytes[0] = byte;
            state = COMMAND2;
            return PARSER_IN_PROGRESS;
        case COMMAND2:
            packet->command.bytes[1] = byte;
            state = TYPE;
            return PARSER_COMPLETE;
        default:
            printf("[command_packet_parser] Reached impossible state: %d!\n", state);
            state = TYPE;
            return PARSER_ERROR;
    }
}

packet_logs_S * packet_get_logs(void)
{
    return &logs;
}