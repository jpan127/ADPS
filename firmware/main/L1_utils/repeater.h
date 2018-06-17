#pragma once

#include "common.h"

/**
 *  @module : repeater
 *
 *  Repeats a function call until the output is true
 */



/**
 *  Callback to repeat
 *  @returns : True for callback was successful, false for not successful
 */
typedef bool (* repeat_CB)(void);

/// Context for a repeat call
typedef struct
{
    const uint32_t num_retries;   ///< Number of retries to execute for
    const uint32_t delay_ms;      ///< Delay in millseconds between callback executions
    const repeat_CB callback;     ///< Function to call repeatedly
} repeat_S;

/**
 *  Repeatedly executes the callback until the retries run out or it returns true
 *  @param repeater : Context for the repeat call
 *  @returns        : True if the callback was successful at any point, false if never successful
 */
bool repeater_execute(const repeat_S * const repeater);
