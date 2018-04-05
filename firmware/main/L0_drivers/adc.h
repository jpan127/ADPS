#pragma once
// Framework libraries
#include "driver/adc.h"
// Project libraries
#include "gpio.h"
#include "common.h"



/**
 *  @module : adc1
 *
 *  Initializes and samples from the ADC1 peripheral across all channels
 *  The inputs to this API is a gpio enumeration which maps directly and uniquely to an ADC1 channel
 */

/**
 *  Initializes an ADC1 channel
 *  @param gpio : The GPIO to select as an ADC mux and map to an ADC channel
 *  @returns    : True for successful, flase for not successful
 */
bool adc1_initialize(const gpio_E gpio);

/**
 *  Samples from the ADC register
 *  @param gpio : The GPIO to map to the ADC channel to read from
 *  @returns    : ADC value in volts, negative represents an invalid sample
 */
int32_t acd1_sample(const gpio_E gpio);

/**
 *  Checks if a certain GPIO is initializd for ADC1
 *  @param gpio : The GPIO to check
 *  @returns    : True for initialized, false for not
 */
bool adc1_is_initialized(const gpio_E gpio);
