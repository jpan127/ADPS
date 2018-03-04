#pragma once
// Framework libraries
#include "driver/mcpwm.h"
// Project libraries
#include "gpio.h"



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

/// Enumerate whether to generate pwm using percentage duty or time based duty
typedef enum
{
    pwm_duty_percent = 0,
    pwm_duty_us,
} pwm_duty_type_E;

typedef union
{
    float percent;
    uint32_t us;
} pwm_duty_U;

/// Pair of unit and timer which are required for every pwm operation
typedef struct
{
    mcpwm_unit_t unit;      ///< The pwm unit
    mcpwm_timer_t timer;    ///< The timer to control the PWM signals
} pwm_S;

/// Struct to package configuration parameters
typedef struct
{
    gpios_E pwm_a;          ///< GPIO to output for PWM A
    gpios_E pwm_b;          ///< GPIO to output for PWM B
    pwm_S pwm;              ///< Pair of pwm unit and timer
    uint32_t frequency;     ///< Frequency of PWM signal
} pwm_config_S;

/**
 *  Initialize a PWM module
 *  @param config : Configuration parameters
 */
void pwm_init(const pwm_config_S * config);

/**
 *  Starts a PWM module
 *  @param pwm_pair : Pair of unit and timer specific to the chosen pwm
 */
void pwm_start(pwm_S * pwm_pair);

/**
 *  Generate a duty cycle for the specified PWM module
 *  @param pwm_pair   : Pair of unit and timer specific to the chosen pwm
 *  @param pwm        : Which PWM of the specified module to target
 *  @param duty       : A duty percentage 0.00f - 100.00f or a microsecond whole number
 *  @param percentage : True to interpret duty as a float, false to interpret as uint32_t microseconds
 */
void pwm_generate(pwm_S * pwm_pair, pwm_E pwm, pwm_duty_U duty, pwm_duty_type_E duty_type);

/**
 *  Initialize a PWM module
 *  @param pwm_pair : Pair of unit and timer specific to the chosen pwm
 *  @param pwm      : Which PWM of the specified module to target
 */
void pwm_stop(pwm_S * pwm_pair, pwm_E pwm);
