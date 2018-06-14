#include "gpio.h"


// Struct to store information about each GPIO
typedef struct
{
    gpio_num_t pin;
    gpio_mode_t mode;
} gpio_S;

// Array of GPIOs to initialize and enumerate all used pins
static gpio_S gpio_map[GPIO_NUM_MAX] =
{
    /// Initialize
    [gpio_wheels_en_a]   = { .pin = GPIO_NUM_17 , .mode = GPIO_MODE_OUTPUT },
    [gpio_wheels_en_b]   = { .pin = GPIO_NUM_4  , .mode = GPIO_MODE_OUTPUT },
    [gpio_delivery_en_a] = { .pin = GPIO_NUM_23 , .mode = GPIO_MODE_OUTPUT },
    [gpio_delivery_en_b] = { .pin = GPIO_NUM_22 , .mode = GPIO_MODE_OUTPUT },

    /// Don't initialize
    [gpio_servo_pwm]              = { .pin = GPIO_NUM_18 },
    [gpio_wheels_pwm_a]           = { .pin = GPIO_NUM_16 },
    [gpio_wheels_pwm_b]           = { .pin = GPIO_NUM_0  },
    [gpio_delivery_pwm]           = { .pin = GPIO_NUM_21 },
    [gpio_adc_infrared_bottom]    = { .pin = GPIO_NUM_35 }, ///< GPIO_NUM_36 is busted, gets residual ADC readings from nearby GPIO
    [gpio_adc_infrared_top_left]  = { .pin = GPIO_NUM_39 },
    [gpio_adc_infrared_top_right] = { .pin = GPIO_NUM_34 },
};

void gpio_init(void)
{
    // Base configuration
    gpio_config_t config =
    {
        .intr_type    = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
    };

    // Initialize gpio
    for (gpio_E gpio = gpio_first_invalid + 1; gpio < gpio_last_invalid; gpio++)
    {
        config.mode = gpio_map[gpio].mode;
        config.pin_bit_mask = (1 << (uint32_t)gpio_map[gpio].pin);
        ESP_ERROR_CHECK(gpio_config(&config));
    }
}

uint32_t gpio_get_pin_number(const gpio_E gpio)
{
    if (gpio < gpio_last_invalid)
    {
        return (uint32_t)gpio_map[gpio].pin;
    }
    else
    {
        return gpio_last_invalid;
    }
}

void gpio_set_output_value(const gpio_E gpio, const bool value)
{
    if (gpio < gpio_last_invalid)
    {
        ESP_ERROR_CHECK(gpio_set_level(gpio_map[gpio].pin, value));
    }
}