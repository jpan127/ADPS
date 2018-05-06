#pragma once
/// Framework libraries
#include "driver/gpio.h"
#include "driver/mcpwm.h"
// Project libraries
#include "pwm.h"



/**
 *  @module      : motor
 *  @description : Configures and drives PWM motors
 */

/// Enumerates each PWM motor
typedef enum
{
    motor_wheels = 0,
    motor_delivery,
    motor_servo,
    motor_max,
} motor_E;

/// Enumerates the type of movement
typedef enum
{
    motor_dir_both_forward = 0,
    motor_dir_both_backward,
    motor_dir_a_forward,    ///< A = left @TODO : Fix this
    motor_dir_a_backward,   ///< A = left
    motor_dir_b_forward,    ///< B = right
    motor_dir_b_backward,   ///< B = right
    motor_dir_left,
    motor_dir_right,
    motor_dir_delivery_forward,
    motor_dir_delivery_backward,
    motor_dir_pivot_left,
    motor_dir_pivot_right,
} motor_direction_E;

/// Enumerates whether to increment or decrement duty
typedef enum
{
    duty_increment = 0,
    duty_decrement,
} duty_adjust_E;

/// Struct to configure each motor and to use for enabling / disabling
typedef struct
{
    gpio_E dir_a;          ///< GPIO for PWM operator A direction
    gpio_E dir_b;          ///< GPIO for PWM operator B direction
    pwm_config_S config;   ///< Configuration for PWM
} motor_config_S;

/// Struct for storing duty values of both PWM A and PWM B
typedef struct
{
    float a;
    float b;
} duty_S;

/// Struct for logging duty percentages of all motors
typedef struct
{
    duty_S duty[motor_max];   
} motor_logs_S;

/**
 *  Initializes a motor and stores a pointer to its configuration
 *  @param motor  : Which motor to initialize
 *  @param config : Configuration struct
 *  @returns      : Success status
 */
bool motor_init(const motor_E motor, const motor_config_S * config);

/**
 *  Sets the PWM to move in a specific direction or pattern
 *  @param motor     : The motor to move
 *  @param direction : The type of movement
 *  @param duty      : The duty percentage
 */
void motor_move(motor_E motor, motor_direction_E direction, float duty);

/**
 *  Adjusts the duty cycle by decrementing by a step size
 *  @param motor       : The motor to move
 *  @param direction   : The type of movement
 *  @param step        : The step size
 *  @param adjust_type : Increment or decrement
 */
void motor_adjust_duty(motor_E motor, motor_direction_E direction, float step, duty_adjust_E adjust_type);

/**
 *  Stops the motor by setting duty to 0%
 *  @param motor : The motor to stop
 */
void motor_stop(motor_E motor);

/// Returns true if the wheels are paused, false if not
bool motor_are_wheels_paused(void);

/// Pauses the wheels and saves the duties
void motor_pause_wheels(void);

/// Resumes the wheels and restores the duties
void motor_resume_wheels(void);

/// Returns a pointer to the motor logging struct
motor_logs_S * motor_get_logs(void);
