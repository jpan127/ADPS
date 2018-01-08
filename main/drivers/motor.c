#include "motor.h"
#include "pwm.h"
#include "gpio.h"


// Map of pointers to motor structs
static motor_S *motor_map[MOTORS_MAX] = { NULL };

void motor_init(motor_controller_E controller, motor_S *motor)
{
    // Point pointer to passed in motor
    motor_map[controller] = motor;
    // Initialize PWM
    pwm_init(motor->pwm_unit, motor->pwm_a, motor->pwm_b, motor->timer);
    // Start PWM
    pwm_start(motor->pwm_unit, motor->timer);
}

void motor_move(motor_controller_E controller, motor_direction_E direction, float duty)
{
    if (!motor_map[controller])
    {
        ESP_LOGE("motor_move", "Motor #%d is not initialized", controller);
    }
    else
    {
        switch (direction)
        {
            case MOTOR_DIRECTION_FORWARD:
                gpio_set_output_value(motor_map[controller]->en_a, true);
                gpio_set_output_value(motor_map[controller]->en_b, true);
                pwm_generate(motor_map[controller]->pwm_unit, motor_map[controller]->timer, PWM_AB, duty);
                break;
            case MOTOR_DIRECTION_BACKWARD:
                gpio_set_output_value(motor_map[controller]->en_a, false);
                gpio_set_output_value(motor_map[controller]->en_b, false);
                pwm_generate(motor_map[controller]->pwm_unit, motor_map[controller]->timer, PWM_AB, duty);
                break;
            case MOTOR_DIRECTION_LEFT:
                gpio_set_output_value(motor_map[controller]->en_a, false);
                gpio_set_output_value(motor_map[controller]->en_b, true);
                pwm_generate(motor_map[controller]->pwm_unit, motor_map[controller]->timer, PWM_A, 0);
                pwm_generate(motor_map[controller]->pwm_unit, motor_map[controller]->timer, PWM_B, duty);
                break;
            case MOTOR_DIRECTION_RIGHT:
                gpio_set_output_value(motor_map[controller]->en_a, true);
                gpio_set_output_value(motor_map[controller]->en_b, false);
                pwm_generate(motor_map[controller]->pwm_unit, motor_map[controller]->timer, PWM_A, duty);
                pwm_generate(motor_map[controller]->pwm_unit, motor_map[controller]->timer, PWM_B, 0);
                break;
            default:
                ESP_LOGE("motor_move", "Impossible direction selected: %d", direction);
                break;
        }
    }
}

void motor_stop(motor_controller_E controller)
{
    if (!motor_map[controller])
    {
        ESP_LOGE("motor_stop", "Motor #%d is not initialized", controller);
    }
    else
    {
        pwm_generate(motor_map[controller]->pwm_unit, motor_map[controller]->timer, PWM_AB, 0);
    }
}