#pragma once
// Framework libraries
#include "driver/adc.h"
// Project libraries
#include "gpio.h"
#include "common.h"


/**
 *  Initializes an ADC1 channel
 *  @param gpio : The GPIO to select as an ADC mux and map to an ADC channel
 */
void adc1_initialize(const gpio_E gpio);

/**
 *  Samples from the ADC register
 *  @param gpio : The GPIO to map to the ADC channel to read from
 *  @returns    : ADC value in volts
 */
int32_t acd1_sample(const gpio_E gpio);

/**
 *  Checks if a certain GPIO is initializd for ADC1
 *  @param gpio : The GPIO to check
 *  @returns    : True for initialized, false for not
 */
bool adc1_is_initialized(const gpio_E gpio);
