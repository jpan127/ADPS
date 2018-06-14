#pragma once

#include "common.h"

/**
 *  @module : leaky_bucket
 *
 *  Represents a leaky bucket in which the output rate is slower than the input rate,
 *  as such, some condition is much easier to trigger than it is to clear
 *  The condition to trigger stays triggered until the "water level" drops below a certain point
 */



/// Context for a leaky bucket
typedef struct
{
    const uint64_t input_rate;  ///< Flow of some arbitrary unit into the bucket
    const uint64_t output_rate; ///< Flow of some arbitrary unit out of the bucket
    const uint64_t max_level;   ///< Level in which the bucket is deemed full
    uint64_t current_level;     ///< Current level of the unit in the bucket
} leaky_bucket_S;

/**
 *  Leak the bucket with the output rate
 *  @param lbucket : The leaky bucket context
 */
void lbucket_leak(leaky_bucket_S * const lbucket);

/**
 *  Fill the bucket with the input rate
 *  @param lbucket : The leaky bucket context
 */
void lbucket_fill(leaky_bucket_S * const lbucket);

/**
 *  Check if the bucket is empty
 *  @param lbucket : The leaky bucket context
 *  @returns       : True for empty, false for not empty
 */
bool lbucket_is_empty(leaky_bucket_S * const lbucket);

/**
 *  Check if the bucket is full
 *  @param lbucket : The leaky bucket context
 *  @returns       : True for full, false for not full
 */
bool lbucket_is_full(leaky_bucket_S * const lbucket);
