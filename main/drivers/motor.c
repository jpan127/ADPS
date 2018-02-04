#include "motor.h"
// Project libraries
#include "packet.h"
#include "pwm.h"
#include "gpio.h"



/// Map of pointers to motor structs
static motor_config_S * motor_map[MOTORS_MAX] = { NULL };

/// Motor logs
static motor_logs_S logs =
{
    .duty = { { 0 } },
};

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
            logs.duty[motor].left  = duty;
            logs.duty[motor].right = duty;
            break;
        case MOTOR_DIRECTION_BACKWARD:
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_AB, duty);
            logs.duty[motor].left  = duty;
            logs.duty[motor].right = duty;
            break;
        case MOTOR_DIRECTION_LEFT:
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, true);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_A, 0);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_B, duty);
            logs.duty[motor].left  = 0;
            logs.duty[motor].right = duty;
            break;
        case MOTOR_DIRECTION_RIGHT:
            gpio_set_output_value(motor_map[motor]->en_a, true);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_A, duty);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, PWM_B, 0);
            logs.duty[motor].left  = duty;
            logs.duty[motor].right = 0;
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
    logs.duty[motor].left  = 0;
    logs.duty[motor].right = 0;
}

motor_logs_S * motor_get_logs(void)
{
    return &logs;
}
