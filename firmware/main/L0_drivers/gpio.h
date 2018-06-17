#pragma once
// Framework libraries
#include "driver/gpio.h"
// Project libraries
#include "common.h"



/**
 *  @module      : gpio
 *  @description : Handles configuring and interfacing with gpios, some wrappers over [driver/gpio.h]
 */

/// Setting the gpio config value to this means don't initialize it, any gpio operations on this will also be ignored
#define GPIO_NOT_USING (gpio_last_invalid)

/// Enumerate all used GPIOs
typedef enum
{
    gpio_first_invalid = 0,

    gpio_wheels_en_a,
    gpio_wheels_en_b,
    gpio_delivery_en_a,
    gpio_delivery_en_b,

    gpio_last_invalid,

    /// Do not initialize as gpio
    gpio_wheels_pwm_a,
    gpio_wheels_pwm_b,
    gpio_servo_pwm,
    gpio_delivery_pwm,
    gpio_adc_infrared_bottom,
    gpio_adc_infrared_top_left,
    gpio_adc_infrared_top_right,
} gpio_E;

/// Initializes all the gpios in [gpio_E]
void gpio_init(void);

/**
 *  Looks up the pin number of an enumerated gpio
 *  @param gpio : The gpio to lookup
 *  @returns    : The pin number as as integer
 */
uint32_t gpio_get_pin_number(gpio_E gpio);

/**
 *  Sets the state of an output GPIO
 *  @param gpi   : The gpio to configure
 *  @param value : The state to set to
 */
void gpio_set_output_value(gpio_E gpio, bool value);
