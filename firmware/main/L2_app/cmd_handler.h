#pragma once
// Project libraries
#include "common.h"
#include "packet_structure.h"



/**
 *  @module : cmd_handler
 *  
 *  Determines the appropriate actions based on a command opcode and executes them
 */

/**
 *  Interprets a command packet and calls the respective API
 *  @param packet : Decoded command packet
 */
void cmd_handler_service_command(const command_packet_S * const packet);

void cmd_handler_set_override(bool on);

void navigation_backup(float duty);
