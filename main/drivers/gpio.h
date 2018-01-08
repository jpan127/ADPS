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
    gpio_left_wheel_pwm,
    gpio_left_wheel_en,

    gpio_right_wheel_pwm,
    gpio_right_wheel_en,

    gpio_delivery_pwm,
    gpio_delivery_en,
    gpio_last_invalid,
} gpios_E;

// @description :
void gpio_init(void);

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