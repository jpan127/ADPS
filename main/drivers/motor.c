#include "motor.h"
// Project libraries
#include "packet.h"
#include "pwm.h"
#include "gpio.h"



/// Struct for storing duty vlaues of left and right PWMs
typedef struct
{
    float left;
    float right;
} duty_S;

/// Map of pointers to motor structs
static motor_config_S * motor_map[MOTORS_MAX] = { NULL };

static duty_S motor_get_status(motor_E motor)
{
    const duty_S duty = 
    {
        .left  = mcpwm_get_duty(motor_map[motor]->pwm_unit, motor_map[motor]->timer, MCPWM_OPR_A),
        .right = mcpwm_get_duty(motor_map[motor]->pwm_unit, motor_map[motor]->timer, MCPWM_OPR_B),
    };

    return duty;
}

bool motor_init(motor_E motor, motor_config_S *config)
{
    bool success = false;

    if (MOTOR_FIRST_INVALID != motor && MOTORS_MAX != motor)
    {
        // Store pointer
        motor_map[motor] = config;
        // Initialize PWM
        pwm_init(motor_map[motor]->pwm_unit, motor_map[motor]->pwm_a, motor_map[motor]->pwm_b, motor_map[motor]->timer);
        // Start PWM
        pwm_start(motor_map[motor]->pwm_unit, motor_map[motor]->timer);
        success = true;
    }

    return success;
}

void motor_move(motor_E motor, motor_direction_E direction, float duty)
{
    switch (direction)
    {
        // @TODO : This is not universal for all motors, need to change
        case MOTOR_DIRECTION_FORWARD:
            gpio_set_output_value(motor_map[motor]->en_a, true);
            gpio_set_output_value(motor_map[motor]->en_b, true);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_AB, duty);
            break;
        case MOTOR_DIRECTION_BACKWARD:
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_AB, duty);
            break;
        case MOTOR_DIRECTION_LEFT:
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, true);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_A, 0);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_B, duty);
            break;
        case MOTOR_DIRECTION_RIGHT:
            gpio_set_output_value(motor_map[motor]->en_a, true);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_A, duty);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_B, 0);
            break;
        default:
            ESP_LOGE("motor_move", "Impossible direction selected: %d", direction);
            break;
    }
}

void motor_stop(motor_E motor)
{
    const float zero_duty = 0.0f;
    pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_AB, zero_duty);
}

void motor_log_status(void)
{
    for (motor_E motor = MOTOR_FIRST_INVALID+1; motor < MOTORS_MAX; motor++)
    {
        const duty_S duty = motor_get_status(motor);
        log_data("motor,%d,%f,%f", motor, duty.left, duty.right);
    }
}