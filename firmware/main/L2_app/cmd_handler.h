#pragma once
// Project libraries
#include "common.h"
#include "packet_structure.h"
#include "freertos/semphr.h"

/**
 *  @module : cmd_handler
 *  
 *  Determines the appropriate actions based on a command opcode and executes them
 */



/// The states that specify which actions happen during navigation
typedef enum
{
    navigation_state_navigating_sidewalk = 0,
    navigation_state_looking_for_path    = 1,
    navigation_state_found_path_to_house = 2,
    navigation_state_navigating_path     = 3,
    navigation_state_reached_door        = 4,
    navigation_state_deliver_package     = 5,
    navigation_state_last_invalid,
} navigation_state_E;

extern SemaphoreHandle_t self_test_sem;

/**
 *  Interprets a command packet and calls the respective API
 *  @param packet : Decoded command packet
 */
void cmd_handler_service_command(const command_packet_S * const packet);

void cmd_handler_set_override(bool on);

void navigation_backup(float duty);

navigation_state_E navigation_get_state(void);

void deliver_package(void);
