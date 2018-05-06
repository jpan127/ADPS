#pragma once
// Project libraries
#include "common.h"
#include "packet_structure.h"
#include "navigation.h"

/**
 *  @module : cmd_handler
 *  
 *  Determines the appropriate actions based on a command opcode and executes them
 */



/// @ { Disables / enables external commands, helper macro for setting override flag
#define DISABLE_EXTERNAL_COMMANDS() cmd_handler_set_override(true)
#define ENABLE_EXTERNAL_COMMANDS()  cmd_handler_set_override(false)
/// @ }

/**
 *  Interprets a command packet and calls the respective API
 *  Checks first if the system is in override mode, if true, ignore commands
 *  Do not need to worry about subsequent commands interrupting when servicing the current commands because
 *   this function serializes all external commands
 *  @param packet : Decoded command packet
 */
void cmd_handler_service_command(const command_packet_S * const packet);

/**
 *  Enables override for the firmware to take over controls instead of servicing external commands
 *  @param on : True for enable override, false for disable
 *  @note     : Essentially acts like a enable / disable interrupts for commands
 */
void cmd_handler_set_override(const bool on);

command_packet_S * cmd_handler_get_last_packet(void);
