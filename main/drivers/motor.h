#pragma once
#include "driver/gpio.h"
#include "driver/mcpwm.h"


/**
 *  @module      : motor
 *  @description : Configures and drives PWM motors
 */


typedef enum
{
    MOTOR_0, // TODO : Change naming to be explicit
    MOTOR_1,
    MOTOR_2,
    MOTORS_MAX
} motor_controller_E;

typedef enum
{
    MOTOR_DIRECTION_FORWARD,
    MOTOR_DIRECTION_BACKWARD,
    MOTOR_DIRECTION_LEFT,
    MOTOR_DIRECTION_RIGHT,
    MOTOR_DIRECTION_STOP
} motor_direction_E;

typedef struct
{
    gpio_num_t en_a;
    gpio_num_t en_b;
    gpio_num_t pwm_a;
    gpio_num_t pwm_b;
    mcpwm_unit_t pwm_unit;
    mcpwm_timer_t timer;
} motor_S;

void motor_init(motor_controller_E controller, motor_S *motor);

void motor_move(motor_controller_E controller, motor_direction_E direction, float duty);

void motor_stop(motor_controller_E controller);