#pragma once
#include "gpio.h"
#include "common.h"



/**
 *  Initializes the infrared and performs a quick self test
 *  @param gpio : The ADC GPIO to initialize
 *  @returns    : True for self test passed, false for self test failed
 */
bool infrared_initialize(const gpio_E gpio);

/**
 *  Samples the ADC value multiple times with a delay in between sampling
 *  @param gpio    : The ADC GPIO to sample
 *  @param samples : The number of samples to take
 *  @param delay   : The delay between samples
 *  @returns       : The average of all the samples
 */
uint32_t infrared_burst_sample(const gpio_E gpio, const uint8_t samples, const uint16_t delay_us);
