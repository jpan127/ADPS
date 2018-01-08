#pragma once
// Framework libraries
#include "driver/mcpwm.h"


/**
 *  @module      : pwm
 *  @description : Driver for controlling the chip's PWM peripherals
 */


typedef enum 
{
    PWM_A,
    PWM_B,
    PWM_AB
} pwm_operator_E;

void pwm_init(mcpwm_unit_t pwm_unit, gpio_num_t gpio_a, gpio_num_t gpio_b, mcpwm_timer_t timer);

void pwm_set_frequency(mcpwm_unit_t pwm_unit, uint32_t frequency);

void pwm_set_duty_a(mcpwm_unit_t pwm_unit, uint32_t duty);

void pwm_set_duty_b(mcpwm_unit_t pwm_unit, uint32_t duty);

void pwm_configure(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer);

void pwm_start(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer);

void pwm_generate(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer, pwm_operator_E pwm_output, float duty);

void pwm_stop(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer, pwm_operator_E pwm_output);