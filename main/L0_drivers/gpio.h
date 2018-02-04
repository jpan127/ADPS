#pragma once
// Framework libraries
#include "driver/gpio.h"
// Project libraries
#include "common.h"



/**
 *  @module      : gpio
 *  @description : Handles configuring and interfacing with gpios, some wrappers over [driver/gpio.h]
 */

/// Enumerate all used GPIOs
typedef enum
{
    gpio_first_invalid = 0,

    gpio_wheels_en_a,
    gpio_wheels_en_b,
    gpio_wheels_pwm_a,
    gpio_wheels_pwm_b,

    gpio_last_invalid,
} gpios_E;

/// Initializes all the gpios in [gpios_E]
void gpio_init(void);

/**
 *  Looks up the pin number of an enumerated gpio
 *  @param gpio : The gpio to lookup
 *  @returns    : The pin number as as integer
 */
uint32_t gpio_get_pin_number(gpios_E gpio);

/**
 *  Sets a GPIO for interrupt mode
 *  @param pin  : The gpio to setup
 *  @param type : The type of interrupt
 */
void gpio_set_interrupt(gpio_num_t pin, gpio_int_type_t type);

/**
 *  Sets a pin for pullup / pulldown resistor configuration
 *  @param pin  : The gpio to configure
 *  @param mode : The resistor mode to set to
 */
void gpio_set_resistor_mode(gpio_num_t pin, gpio_pull_mode_t mode);

/**
 *  Sets the state of an output GPIO
 *  @param pin   : The gpio to configure
 *  @param value : The state to set to
 */
void gpio_set_output_value(gpio_num_t pin, bool value);
