#pragma once
#include "driver/gpio.h"
#include "driver/mcpwm.h"



/**
 *  @module      : motor
 *  @description : Configures and drives PWM motors
 */

/// Enumerates each PWM motor
typedef enum
{
    MOTOR_FIRST_INVALID = 0,
    MOTOR_WHEELS,
    MOTOR_SHAFT,
    MOTORS_MAX
} motor_E;

/// Enumerates the type of movement
typedef enum
{
    MOTOR_DIRECTION_FORWARD,
    MOTOR_DIRECTION_BACKWARD,
    MOTOR_DIRECTION_LEFT,
    MOTOR_DIRECTION_RIGHT,
    MOTOR_DIRECTION_STOP
} motor_direction_E;

/// Struct to configure each motor and to use for enabling / disabling
typedef struct
{
    gpio_num_t en_a;        ///< GPIO for left wheel enable
    gpio_num_t en_b;        ///< GPIO for right wheel enable
    gpio_num_t pwm_a;       ///< GPIO for left wheel PWM
    gpio_num_t pwm_b;       ///< GPIO for right wheel PWM
    mcpwm_unit_t pwm_unit;  ///< Specifies the PWM unit
    mcpwm_timer_t timer;    ///< Specifies the hardware timer to control the PWM
} motor_config_S;

/// Struct for storing duty vlaues of left and right PWMs
typedef struct
{
    float left;
    float right;
} duty_S;

/// Struct for logging duty percentages of all motors
typedef struct
{
    duty_S duty[MOTORS_MAX];   
} motor_logs_S;

/**
 *  Initializes a motor and stores a pointer to its configuration
 *  @param motor  : Which motor to initialize
 *  @param config : Configuration struct
 *  @returns      : Success status
 */
bool motor_init(motor_E motor, motor_config_S *config);

/**
 *  Sets the PWM to move in a specific direction or pattern
 *  @param motor     : The motor to move
 *  @param direction : The type of movement
 *  @param duty      : The duty percentage
 */
void motor_move(motor_E motor, motor_direction_E direction, float duty);

/**
 *  Adjusts the duty cycle by incrementing by a step size
 *  @param motor     : The motor to move
 *  @param direction : The type of movement
 *  @param step      : The step size
 */
void motor_increment(motor_E motor, motor_direction_E direction, float step);

/**
 *  Adjusts the duty cycle by decrementing by a step size
 *  @param motor     : The motor to move
 *  @param direction : The type of movement
 *  @param step      : The step size
 */
void motor_deccrement(motor_E motor, motor_direction_E direction, float step);

/**
 *  Stops the motor by setting duty to 0%
 *  @param motor : The motor to stop
 */
void motor_stop(motor_E motor);

/// Returns a pointer to the motor logging struct
motor_logs_S * motor_get_logs(void);
