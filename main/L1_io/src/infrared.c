#include "infrared.h"
#include "adc.h"



static bool infrared_self_test(const gpio_E gpio)
{
    const uint8_t num_tests = 10;
    uint32_t samples[10] = { 0 };
    uint8_t failures = 0;

    for (uint8_t test = 0; test < num_tests; test++)
    {
        const int32_t sample = acd1_sample(gpio);
        if (sample >= 0)
        {
            ESP_LOGI("infrared_self_test", "Infrared sensor self test : %d", samples[test]);
            samples[test] = sample;
        }
        else
        {
            ++failures;
        }
    }

    return (failures == 0);
}

bool infrared_initialize(const gpio_E gpio)
{
    adc1_initialize(gpio);
    return infrared_self_test(gpio);
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
