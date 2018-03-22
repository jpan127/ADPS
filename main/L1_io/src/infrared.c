#include "infrared.h"
#include "adc.h"



/// Map of saved GPIO handles that maps to each infrared sensor
static gpio_E infrared_map[infrared_max] = { 0 };

/**
 *  Runs a self test on a sensor by sampling from it multiple times and checking for ADC errors
 *  @param ir : The sensor to test
 *  @returns  : True for pass, false for fail
 */
static bool infrared_self_test(const infarred_E ir)
{
    const uint8_t minimum_value_to_not_fault = 10;
    const uint8_t num_tests = 10;
    uint32_t samples[10] = { 0 };
    uint8_t failures = 0;

    for (uint8_t test = 0; test < num_tests; test++)
    {
        const int32_t sample = acd1_sample(infrared_map[ir]);
        if (sample >= minimum_value_to_not_fault)
        {
            ESP_LOGI("infrared_self_test", "[%d] Infrared sensor self test : %d", ir, samples[test]);
            samples[test] = sample;
        }
        else
        {
            ++failures;
        }
    }

    return (failures == 0);
}

/**
 *  Converts the ADC reading in volts to a range
 *  @param voltage : ADC reading
 *  @returns       : Range in centimeters
 */
static float infrared_apply_linearizing_fx(const float voltage)
{
    /**
     *  V = voltage
     *  R = range
     *  y = mx + b where 
     *      y : 1 / (R + k)
     *      m : m
     *      x : V
     *      b : b
     *  1 / (R + k) = mV + b
     *      1 / (mV + b) = R + k
     *      R = (1 / (mV + b)) - k
     */

    // Calibration parameters
    const float m = 1;
    const float b = 0;
    const float k = 0;

    return (1.0f / (m * voltage + b)) - k;
}

void infrared_initialize(const gpio_E * gpio, bool * functional)
{
    for (infrared_E ir = (infrared_E)0; ir < infrared_max; ir++)
    {
        // Save to map
        infrared_map[ir] = gpio[ir];
        
        // Initialize the ADC channel
        if (!adc1_initialize(infrared_map[ir]))
        {
            ESP_LOGE("infrared_initialize", "Failed to initialize infrared sensor %d", ir);
            continue;
        }
        
        // Run self test on the sensor
        if ((function[ir] = infrared_self_test(ir)) == false)
        {
            ESP_LOGE("infrared_initialize", "Failed self test infrared sensor %d", ir);
        }
    }
}

uint32_t infrared_burst_sample(const gpio_E gpio, const uint8_t samples, const uint16_t delay_us)
{
    uint32_t average = 0;

    if (adc1_is_initialized(gpio))
    {
        for (uint8_t sample = 0; sample < samples; sample++)
        {
            const int32_t reading = acd1_sample(gpio);
            if (reading >= 0)
            {
                average += reading;
                ESP_LOGI("infrared_burst_sample", "Sample : %d", reading);
                DELAY_US(delay_us);
            }
#if TESTING
            else
            {
                ESP_LOGE("infrared_burst_sample", "Error reading sample %d", sample);
            }
#endif
        }

        average /= samples;
    }

    return average;
}
