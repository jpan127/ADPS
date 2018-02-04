#pragma once
// Framework libraries
#include "driver/gpio.h"
// Project libraries
#include "common.h"


/**
 *  @module      : gpio
 *  @description : Handles configuring and interfacing with gpios
 */

typedef enum
{
    gpio_first_invalid = 0,

    gpio_wheels_en_a,
    gpio_wheels_en_b,
    gpio_wheels_pwm_a,
    gpio_wheels_pwm_b,

    gpio_last_invalid,
} gpios_E;

// @description :
void gpio_init(void);

uint32_t gpio_get_pin_number(gpios_E gpio);

// @description :
// @param pin   :
// @param type  :
void gpio_set_interrupt(gpio_num_t pin, gpio_int_type_t type);

// @description :
// @param pin   :
// @param mode  :
void gpio_set_resistor_mode(gpio_num_t pin, gpio_pull_mode_t mode);

// @description :
// @param pin   :
// @param value :
void gpio_set_output_value(gpio_num_t pin, bool value);