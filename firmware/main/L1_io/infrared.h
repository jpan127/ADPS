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
    infrared_bottom = 0,
    infrared_top_left,
    infrared_top_right,
    infrared_max,
} infrared_E;

typedef struct
{
    float distance_cm[infrared_max];
} infrared_readings_S;

typedef struct
{
    bool operational;       ///< Infrared sensor passed self test
    uint32_t raw_values;    ///< Last raw reading
    float distances;        ///< Last distance reading
} infrared_logs_S;

/**
 *  Initializes the infrared and performs a quick self test
 *  @param gpio       : The ADC GPIOs to initialize, expects a size of [infrared_max]
 *  @param functional : An array of pass/fail results of self testing each sensor, expects a size of [infrared_max]
 *  @returns          : True for self test passed, false for self test failed
 */
void infrared_initialize(const gpio_E * gpio, bool * functional);

/**
 *  Samples the ADC value multiple times with a delay in between sampling
 *  @param ir       : The infrared sensor to sample
 *  @param samples  : The number of samples to take
 *  @param delay_ms : The delay between samples
 *  @returns        : The average of all the samples in distance in cm
 *  @note           : The sensors have an average of ~40ms between readings
 */
float infrared_burst_sample(const infrared_E ir, const uint8_t samples, uint16_t delay_ms);

/**
 *  Samples each IR sensor with a burst sample and writes into the struct
 *  @param samples  : The number of samples to take
 *  @param delay_ms : The delay between samples
 *  @param readings : Struct to contain all the sensor readings in distance in cm
 */
void infrared_burst_sample_all(const uint8_t samples, const uint16_t delay_ms, infrared_readings_S * const readings);

/// Returns the infrared logging struct
infrared_logs_S * infrared_get_logs(void);
