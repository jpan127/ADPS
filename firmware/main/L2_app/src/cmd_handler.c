#include "cmd_handler.h"

#include "motor.h"
#include "tasks.h"



/// Flag for enabling manual controls or not
static bool manual_mode_enabled = false;

/// Flag for currently firmware controlling navigation and ignoring external commands
static bool override_in_progress = false;

/// Saves the last command packet, so that when override is disabled it can resume immediately the last packet
static command_packet_S last_packet = { 0 };

void cmd_handler_set_override(const bool on)
{
    override_in_progress = on;
}

command_packet_S * cmd_handler_get_last_packet(void)
{
    return &last_packet;
}

void cmd_handler_service_command(const command_packet_S * const packet)
{
    // Save last packet
    last_packet = *packet;

    // Ignore commands when override is in progress
    if (!override_in_progress)
    {
        if (packet_opcode_servo_duty == packet->opcode)
        {
            xSemaphoreGive(ServoSemaphore);
        }
        else if (packet_opcode_deliver == packet->opcode)
        {
            xSemaphoreGive(DeliverySemaphore);
        }
        else if (packet_opcode_manual_mode == packet->opcode)
        {
            ESP_LOGI("COMMAND", "packet_opcode_manual_mode"); 
            manual_mode_enabled = (packet->command[0] != 0);
        }
        else if (packet_opcode_change_state == packet->opcode)
        {
            if (packet->command[0] < navigation_state_last_invalid)
            {
                navigation_set_state((navigation_state_E)(packet->command[0]));
            }
        }
        else if (packet_opcode_execute_self_test == packet->opcode)
        {
            xSemaphoreGive(SelfTestSemaphore);
        }
        else if (packet->opcode < packet_opcode_last_invalid)
        {
            xSemaphoreGive(NavigationSemaphore);
        }
    }
}
