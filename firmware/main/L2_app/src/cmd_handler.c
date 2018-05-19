#include "cmd_handler.h"

#include "motor.h"
#include "tasks.h"



/// Flag for enabling manual controls or not
static bool manual_mode_enabled = false;

/// Flag for currently firmware controlling navigation and ignoring external commands
static bool override_in_progress = false;

/// Saves the last navigation command packet, so that when override is disabled it can resume immediately the last packet
static command_packet_S last_navigation_packet = { 0 };

/// Saves the last servo duty to be sent to task_servo
static uint8_t last_commanded_servo_duty = 0;

uint8_t cmd_handler_get_last_commanded_servo_duty(void)
{
    return last_commanded_servo_duty;
}

void cmd_handler_set_override(const bool on)
{
    override_in_progress = on;
}

command_packet_S * cmd_handler_get_last_navigation_packet(void)
{
    return &last_navigation_packet;
}

void cmd_handler_service_command(const command_packet_S * const packet)
{
    // Ignore commands when override is in progress
    if (!override_in_progress)
    {
        switch (packet->opcode)
        {
            case packet_opcode_set_suspend       : { set_suspend_state((packet->opcode != 0));                                       break; }
            case packet_opcode_servo_duty        : { xSemaphoreGive(ServoSemaphore); last_commanded_servo_duty = packet->command[0]; break; }
            case packet_opcode_deliver           : { xSemaphoreGive(DeliverySemaphore);                                              break; }
            case packet_opcode_manual_mode       : { manual_mode_enabled = (packet->command[0] != 0);                                break; }
            case packet_opcode_change_state      : { navigation_set_state((navigation_state_E)(packet->command[0]));                 break; }
            case packet_opcode_execute_self_test : { xSemaphoreGive(SelfTestSemaphore);                                              break; }
            default:
            {
                // Save last packet
                last_navigation_packet = *packet;
                // Signal to task_navigation
                xSemaphoreGive(NavigationSemaphore);
            }
        }
    }
}
