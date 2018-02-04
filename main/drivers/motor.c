#include "motor.h"
// Project libraries
#include "packet.h"
#include "pwm.h"
#include "gpio.h"



/// Map of pointers to motor structs
static motor_config_S * motor_map[MOTORS_MAX] = { NULL };

/// Motor logs
static motor_logs_S logs = { .duty = { { 0 } } };

/// Assigning a name to each PWM for clarity
static const pwm_E left  = PWM_A;
static const pwm_E right = PWM_B;
static const pwm_E both  = PWM_AB;

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
        case MOTOR_DIRECTION_FORWARD:
            gpio_set_output_value(motor_map[motor]->en_a, true);
            gpio_set_output_value(motor_map[motor]->en_b, true);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, both, duty);
            logs.duty[motor].left  = duty;
            logs.duty[motor].right = duty;
            break;
        case MOTOR_DIRECTION_BACKWARD:
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, both, duty);
            logs.duty[motor].left  = duty;
            logs.duty[motor].right = duty;
            break;
        case MOTOR_DIRECTION_LEFT:
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, true);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, left, 0);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, right, duty);
            logs.duty[motor].left  = 0;
            logs.duty[motor].right = duty;
            break;
        case MOTOR_DIRECTION_RIGHT:
            gpio_set_output_value(motor_map[motor]->en_a, true);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, left, duty);
            pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, right, 0);
            logs.duty[motor].left  = duty;
            logs.duty[motor].right = 0;
            break;
        default:
            ESP_LOGE("motor_move", "Impossible direction selected: %d", direction);
            break;
    }
}

void motor_increment(motor_E motor, motor_direction_E direction, float step)
{
    /**
     *  Limit duty to 100.0f, before setting, even though underneath pwm limits it already, this way the current values in [logs] are accurate
     *  Always set both PWM values even if they aren't incrementing, so the switch statement is simplified
     */
    switch (direction)
    {
        case MOTOR_DIRECTION_FORWARD:
            logs.duty[motor].left  = MIN(logs.duty[motor].left  + step, 100.0f);
            logs.duty[motor].right = MIN(logs.duty[motor].right + step, 100.0f);
            gpio_set_output_value(motor_map[motor]->en_a, true);
            gpio_set_output_value(motor_map[motor]->en_b, true);
            break;
        case MOTOR_DIRECTION_BACKWARD:
            logs.duty[motor].left  = MIN(logs.duty[motor].left  + step, 100.0f);
            logs.duty[motor].right = MIN(logs.duty[motor].right + step, 100.0f);
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            break;
        case MOTOR_DIRECTION_LEFT:
            logs.duty[motor].left  = 0;
            logs.duty[motor].right = MIN(logs.duty[motor].right + step, 100.0f);
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, true);
            break;
        case MOTOR_DIRECTION_RIGHT:
            logs.duty[motor].left  = MIN(logs.duty[motor].left  + step, 100.0f);
            logs.duty[motor].right = 0;
            gpio_set_output_value(motor_map[motor]->en_a, true);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            break;
        default:
            ESP_LOGE("motor_move", "Impossible direction selected: %d", direction);
            break;
    }

    pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer,  left, logs.duty[motor].left);
    pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, right, logs.duty[motor].right);
}

void motor_deccrement(motor_E motor, motor_direction_E direction, float step)
{
    /**
     *  Limit to positive, before setting, even though underneath pwm limits it already, this way the current values in [logs] are accurate
     *  Always set both PWM values even if they aren't incrementing, so the switch statement is simplified
     */
    switch (direction)
    {
        case MOTOR_DIRECTION_FORWARD:
            logs.duty[motor].left  = MAX(logs.duty[motor].left  - step, 0.0f);
            logs.duty[motor].right = MAX(logs.duty[motor].right - step, 0.0f);
            gpio_set_output_value(motor_map[motor]->en_a, true);
            gpio_set_output_value(motor_map[motor]->en_b, true);
            break;
        case MOTOR_DIRECTION_BACKWARD:
            logs.duty[motor].left  = MAX(logs.duty[motor].left  - step, 0.0f);
            logs.duty[motor].right = MAX(logs.duty[motor].right - step, 0.0f);
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            break;
        case MOTOR_DIRECTION_LEFT:
            logs.duty[motor].left  = 0;
            logs.duty[motor].right = MAX(logs.duty[motor].right - step, 0.0f);
            gpio_set_output_value(motor_map[motor]->en_a, false);
            gpio_set_output_value(motor_map[motor]->en_b, true);
            break;
        case MOTOR_DIRECTION_RIGHT:
            logs.duty[motor].left  = MAX(logs.duty[motor].left  - step, 0.0f);
            logs.duty[motor].right = 0;
            gpio_set_output_value(motor_map[motor]->en_a, true);
            gpio_set_output_value(motor_map[motor]->en_b, false);
            break;
        default:
            ESP_LOGE("motor_move", "Impossible direction selected: %d", direction);
            break;
    }

    pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer,  left, logs.duty[motor].left);
    pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, right, logs.duty[motor].right);
}

void motor_stop(motor_E motor)
{
    const float zero_duty = 0.0f;
    pwm_generate(motor_map[motor]->pwm_unit, motor_map[motor]->timer, both, zero_duty);
    logs.duty[motor].left  = zero_duty;
    logs.duty[motor].right = zero_duty;
}

motor_logs_S * motor_get_logs(void)
{
    return &logs;
}
