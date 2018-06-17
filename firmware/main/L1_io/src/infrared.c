#include "infrared.h"
#include "adc.h"



/// Map of saved GPIO handles that maps to each infrared sensor
static gpio_E infrared_map[infrared_max] = { 0 };

/// Infrared logs
static infrared_logs_S logs[infrared_max] = { 0 };

/**
 *  Runs a self test on a sensor by sampling from it multiple times and checking for ADC errors
 *  @param ir : The sensor to test
 *  @returns  : True for pass, false for fail
 */
static bool infrared_self_test(const infrared_E ir)
{
    const uint8_t minimum_value_to_not_fault = 100;
    const uint8_t num_tests = 10;
    const uint8_t num_acceptable_failures = (float)num_tests * 0.1F;
    uint8_t failures = 0;

    for (uint8_t test = 0; test < num_tests; test++)
    {
        const int32_t sample = acd1_sample(infrared_map[ir]);
        if (sample >= minimum_value_to_not_fault)
        {
            ESP_LOGI("infrared_self_test", "[%d] Infrared sensor self test : %d", ir, sample);
        }
        else
        {
            // Even if already failed too many times, finish the test
            ++failures;
        }
    }

    return (failures < num_acceptable_failures);
}

/**
 *  Converts the ADC reading in volts to a distance in centimeters
 *  @param voltage : ADC reading
 *  @returns       : Range in centimeters
 */
static float infrared_apply_linearizing_fx(const infrared_E ir, const float voltage)
{
    /**
     *  Linearized distance from voltage:
     *      y = mx + b
     *      voltage = slope * distance_cm + y_intercept
     *      voltage - y_intercept = slope * distance_cm
     *      (voltage - y_intercept) / slope = distance_cm
     *      distance_cm = (voltage - y_intercept) / slope
     */

    // Calibration parameters, characterized offline
    static const float A02_slope = -41.4f;
    static const float A02_y_intercept = 4963.0f;

    /// @TODO : [#12] Additional characterization needed
#if 0
    static const float A21_slope = -15.7f;
    static const float A21_y_intercept = 3290.0f;
    if (infrared_bottom == ir)
    {
        return MAX(0, (voltage - A21_y_intercept) / A21_slope);
    }
    else
    {
        return MAX(0, (voltage - A02_y_intercept) / A02_slope);
    }
#else
    const float distance_cm = (voltage - A02_y_intercept) / A02_slope;
    return MAX(0, distance_cm);
#endif
}

void infrared_initialize(const gpio_E * gpio, bool * functional)
{
    for (infrared_E ir = (infrared_E)0; ir < infrared_max; ir++)
    {
        // Save to map
        infrared_map[ir] = gpio[ir];

        // Start off as passed test
        functional[ir] = true;

        // Only initialize if wasn't properly initialized already
        if (!logs[ir].operational)
        {
            // Initialize the ADC channel
            if (!adc1_initialize(infrared_map[ir]))
            {
                ESP_LOGE("infrared_initialize", "Failed to initialize infrared sensor %d", ir);
                functional[ir] = false;
                continue;
            }
        }

        // Run self test on the sensor
        functional[ir] = infrared_self_test(ir);
        logs[ir].operational = functional[ir];
        if (!functional[ir])
        {
            ESP_LOGE("infrared_initialize", "Failed self test infrared sensor %d", ir);
        }
        else
        {
            ESP_LOGI("infrared_initialize", "Successfully initialized infrared sensor %d", ir);
        }
    }
}

float infrared_burst_sample(const infrared_E ir, const uint8_t samples, uint16_t delay_ms)
{
    const uint8_t minimum_delay_between_readings_ms = 40;

    uint32_t average = 0;

    delay_ms = MAX(minimum_delay_between_readings_ms, delay_ms);

    if (adc1_is_initialized(infrared_map[ir]))
    {
        for (uint8_t sample = 0; sample < samples; sample++)
        {
            const int32_t reading = acd1_sample(infrared_map[ir]);
            if (reading >= 0)
            {
                average += reading;
            }
#if TESTING
            else
            {
                ESP_LOGE("infrared_burst_sample", "Error reading sample %d", sample);
            }
#endif
            DELAY_MS(delay_ms);
        }

        average /= samples;
    }

    const float linearized_averaged_distance_cm = infrared_apply_linearizing_fx(ir, average);

    logs[ir].raw_values = average;
    logs[ir].distances  = linearized_averaged_distance_cm;

    return linearized_averaged_distance_cm;
}

void infrared_burst_sample_all(const uint8_t samples, uint16_t delay_ms, infrared_readings_S * const readings)
{
    for (infrared_E ir = (infrared_E)0; ir < infrared_max; ir++)
    {
        readings->distance_cm[ir] = infrared_burst_sample(ir, samples, delay_ms);
    }
}

infrared_logs_S * infrared_get_logs(void)
{
    return &logs[0];
}
