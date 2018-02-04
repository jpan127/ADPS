#pragma once
// Framework libraries
#include "driver/mcpwm.h"



/**
 *  @module      : pwm
 *  @description : Driver for controlling the chip's PWM peripherals
 */

/// Enumerate which PWM of each module to target
typedef enum 
{
    PWM_A = 0,
    PWM_B,
    PWM_AB
} pwm_E;

/**
 *  Initialize a PWM module
 *  @param pwm_unit : The unit to initialize
 *  @param gpio_a   : GPIO to output for PWM A
 *  @param gpio_b   : GPIO to output for PWM B
 *  @param timer    : The timer to control the PWM signals
 */
void pwm_init(mcpwm_unit_t pwm_unit, gpio_num_t gpio_a, gpio_num_t gpio_b, mcpwm_timer_t timer);

/**
 *  Starts a PWM module
 *  @param pwm_unit : The unit to start
 *  @param timer    : The timer to control the PWM signals
 */
void pwm_start(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer);

/**
 *  Generate a duty cycle for the specified PWM module
 *  @param pwm_unit : The unit to initialize
 *  @param timer    : The timer to control the PWM signals
 *  @param pwm      : Which PWM of the specified module to target
 *  @param duty     : A duty percentage 0.00f - 100.00f
 */
void pwm_generate(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer, pwm_E pwm, float duty);

/**
 *  Initialize a PWM module
 *  @param pwm_unit : The unit to initialize
 *  @param timer    : The timer to control the PWM signals
 *  @param pwm      : Which PWM of the specified module to target
 */
void pwm_stop(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer, pwm_E pwm);
