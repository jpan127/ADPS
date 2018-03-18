#include "adc.h"



static bool adc_init_map[ADC1_CHANNEL_MAX] = { 0 };

static adc1_channel_t adc_gpio_to_channel(const gpio_E gpio)
{
    switch (gpio_get_pin_number(gpio))
    {
        case GPIO_NUM_36: return ADC1_CHANNEL_0;
        case GPIO_NUM_37: return ADC1_CHANNEL_1;
        case GPIO_NUM_38: return ADC1_CHANNEL_2;
        case GPIO_NUM_39: return ADC1_CHANNEL_3;
        case GPIO_NUM_32: return ADC1_CHANNEL_4;
        case GPIO_NUM_33: return ADC1_CHANNEL_5;
        case GPIO_NUM_34: return ADC1_CHANNEL_6;
        case GPIO_NUM_35: return ADC1_CHANNEL_7;
        default:          return ADC1_CHANNEL_MAX;
    }
}

void adc1_initialize(const gpio_E gpio)
{
    const adc1_channel_t channel = adc_gpio_to_channel(gpio);
    
    // If not already initialized
    if (!adc_init_map[channel])
    {
        // Enable GPIO
        const adc_unit_t adc_unit = ADC_UNIT_1;
        adc_gpio_init(channel, adc_unit);

        // Set ADC resolution
        const adc_bits_width_t adc_resolution = ADC_WIDTH_12Bit;
        ESP_ERROR_CHECK(adc1_config_width(adc_resolution));

        // Enable ADC and set attenuation
        const adc_atten_t full_scale_attenuation = ADC_ATTEN_11db;
        ESP_ERROR_CHECK(adc1_config_channel_atten(channel, full_scale_attenuation));
        adc_init_map[channel] = true;
    }
}

int32_t acd1_sample(const gpio_E gpio)
{
    const adc1_channel_t channel = adc_gpio_to_channel(gpio);
    int32_t adc_voltage_v = -1;

    if (adc_init_map[channel])
    {
        adc_voltage_v = adc1_get_raw(channel);
    }

    return adc_voltage_v;
}

bool adc1_is_initialized(const gpio_E gpio)
{
    return adc_init_map[adc_gpio_to_channel(gpio)];
}
