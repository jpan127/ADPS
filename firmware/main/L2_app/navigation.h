#pragma once

#include "common.h"

/**
 *  @module : navigation
 *
 *  Commands to execute navigation control routines
 *  These routines do not set the override mode, that is rather left up to the user
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

/// Sets the system to backup at a specific speed, does not have a timer
void navigation_backup(const float duty, const uint32_t delay_ms);

/// Returns the current state of the state machine on the server
navigation_state_E navigation_get_state(void);

/// Sets the current state of the state machine from the server
void navigation_set_state(const navigation_state_E new_state);

/// Pivots to the left for 1 second
void navigation_pivot_left(void);

/// Pivots to the right for 1 second
void navigation_pivot_right(void);
