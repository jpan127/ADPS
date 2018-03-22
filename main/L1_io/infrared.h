#pragma once
#include "gpio.h"
#include "common.h"



/**
 *  @module : infrared
 *
 *  A simple API to read from an infrared sensor
 *  The sensors must be initialized with a gpio array of [infrared_max] size prior to any sampling
 *  The samples are bursted by a specified size and then are averaged
 */

/// Enumerate the 3 infrared sensors
typedef enum
{
    infrared_1,
    infrared_2,
    infrared_3,
    infrared_max,
} infrared_E;

/**
 *  Initializes the infrared and performs a quick self test
 *  @param gpio       : The ADC GPIOs to initialize, expects a size of [infrared_max]
 *  @param functional : An array of pass/fail results of self testing each sensor, expects a size of [infrared_max]
 *  @returns          : True for self test passed, false for self test failed
 */
void infrared_initialize(const gpio_E * gpio, bool * functional);

/**
 *  Samples the ADC value multiple times with a delay in between sampling
 *  @param gpio    : The ADC GPIO to sample
 *  @param samples : The number of samples to take
 *  @param delay   : The delay between samples
 *  @returns       : The average of all the samples
 */
uint32_t infrared_burst_sample(const gpio_E gpio, const uint8_t samples, const uint16_t delay_us);
