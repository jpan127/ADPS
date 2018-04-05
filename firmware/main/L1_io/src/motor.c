#include "motor.h"
// Project libraries
#include "packet.h"
#include "gpio.h"



/// Map of motor structs
static motor_config_S motor_map[motor_max] = { 0 };

/// Motor logs
static motor_logs_S logs = { .duty = { { { 0 } , { 0 } } } };

/// A duty union of zero
static const pwm_duty_U zero_duty = { 0 };

#define MIN_DUTY_RATIO_US  (500)
#define MAX_DUTY_RATIO_US  (2500)
static const uint16_t duty_ratio_range_us = MAX_DUTY_RATIO_US - MIN_DUTY_RATIO_US;
static const uint8_t  max_degree          = 180;
static const float max_duty_percent       = 100.0f;

/**
 *  Converts a servo degree from a range of [0, max_degree] to a PWM pulse width in microseconds
 *  @param degree : The servo degree to change to
 */
static uint16_t servo_degree_to_pulse_width(uint8_t degree)
{
    // Limit to max_degree
    degree = MIN(degree, max_degree);
    
    const float percentage       = (float)degree / max_degree;
    const uint16_t duty_ratio_us = MIN_DUTY_RATIO_US + (percentage * duty_ratio_range_us);

    return duty_ratio_us;
}

bool motor_init(const motor_E motor, const motor_config_S * config)
{
    bool success = false;

    if (motor_max != motor)
    {
        // Store configuration
        motor_map[motor] = *config;

        // Initialize PWM config
        pwm_init(&motor_map[motor].config);

        // Start PWM
        pwm_start(&motor_map[motor].config.pwm);

        success = true;
    }

    return success;
}

void motor_move(motor_E motor, motor_direction_E direction, float duty)
{
    // Handle conversion from float to microsecond duty
    const pwm_duty_type_E duty_type = (motor_servo == motor) ? (pwm_duty_us) : (pwm_duty_percent);
    const pwm_duty_U percent        = { .percent = duty };
    const pwm_duty_U us             = { .us = servo_degree_to_pulse_width(duty) };
    const pwm_duty_U pwm_duty       = (pwm_duty_percent == duty_type) ? (percent) : (us);

    switch (direction)
    {
        case motor_dir_both_forward:

            gpio_set_output_value(motor_map[motor].dir_a, true);
            gpio_set_output_value(motor_map[motor].dir_b, true);
            pwm_generate(&motor_map[motor].config.pwm, PWM_AB, pwm_duty, duty_type);
            logs.duty[motor].a = pwm_duty;
            logs.duty[motor].b = pwm_duty;
            break;

        case motor_dir_both_backward:

            gpio_set_output_value(motor_map[motor].dir_a, false);
            gpio_set_output_value(motor_map[motor].dir_b, false);
            pwm_generate(&motor_map[motor].config.pwm, PWM_AB, pwm_duty, duty_type);
            logs.duty[motor].a = pwm_duty;
            logs.duty[motor].b = pwm_duty;
            break;

        case motor_dir_a_forward:

            gpio_set_output_value(motor_map[motor].dir_a, true);
            pwm_generate(&motor_map[motor].config.pwm, PWM_A, pwm_duty, duty_type);
            logs.duty[motor].a = pwm_duty;
            break;

        case motor_dir_a_backward:

            gpio_set_output_value(motor_map[motor].dir_a, false);
            pwm_generate(&motor_map[motor].config.pwm, PWM_A, pwm_duty, duty_type);
            logs.duty[motor].a = pwm_duty;
            break;

        case motor_dir_b_forward:

            gpio_set_output_value(motor_map[motor].dir_b, true);
            pwm_generate(&motor_map[motor].config.pwm, PWM_B, pwm_duty, duty_type);
            logs.duty[motor].b = pwm_duty;
            break;

        case motor_dir_b_backward:

            gpio_set_output_value(motor_map[motor].dir_b, false);
            pwm_generate(&motor_map[motor].config.pwm, PWM_B, pwm_duty, duty_type);
            logs.duty[motor].b = pwm_duty;
            break;

        default:

            ESP_LOGE("motor_move", "Impossible direction selected: %d", direction);
            break;
    }

    if (pwm_duty_percent == duty_type)
    {
        ESP_LOGI("motor_move", "Duty : %f %f%%", duty, percent.percent);
    }
    else
    {
        ESP_LOGI("motor_move", "Duty : %f %uus", duty, us.us);
    }
}

void motor_adjust_duty(motor_E motor, motor_direction_E direction, float step, duty_adjust_E adjust_type)
{
    /**
     *  Limit duty to 100.0f, before setting, even though underneath pwm limits it already, this way the current values in [logs] are accurate
     *  Always set both PWM values even if they aren't incrementing, so the switch statement is simplified
     */

    const pwm_duty_type_E duty_type = (motor_servo == motor) ? (pwm_duty_us) : (pwm_duty_percent);

    pwm_duty_U duty_a = { 0 };
    pwm_duty_U duty_b = { 0 };

    // Limit duties
    if (duty_increment == adjust_type)
    {
        if (pwm_duty_percent == duty_type)
        {
            duty_a.percent = MIN(max_duty_percent, logs.duty[motor].a.percent + step);
            duty_b.percent = MIN(max_duty_percent, logs.duty[motor].b.percent + step);
        }
        else
        {
            duty_a.us = MIN(max_degree, logs.duty[motor].a.percent + servo_degree_to_pulse_width(step));
            duty_b.us = MIN(max_degree, logs.duty[motor].b.percent + servo_degree_to_pulse_width(step));
        }
    }
    else
    {
        if (pwm_duty_percent == duty_type)
        {
            duty_a.percent = MAX(0.0f, logs.duty[motor].a.percent - step);
            duty_b.percent = MAX(0.0f, logs.duty[motor].b.percent - step);
        }
        else
        {
            duty_a.us = MAX(0, logs.duty[motor].a.percent - servo_degree_to_pulse_width(step));
            duty_b.us = MAX(0, logs.duty[motor].b.percent - servo_degree_to_pulse_width(step));
        }
    }

    // Set log values first
    switch (direction)
    {
        case motor_dir_both_forward:

            logs.duty[motor].a = duty_a;
            logs.duty[motor].b = duty_b;
            gpio_set_output_value(motor_map[motor].dir_a, true);
            gpio_set_output_value(motor_map[motor].dir_b, true);
            break;

        case motor_dir_both_backward:

            logs.duty[motor].a = duty_a;
            logs.duty[motor].b = duty_b;
            gpio_set_output_value(motor_map[motor].dir_a, false);
            gpio_set_output_value(motor_map[motor].dir_b, false);
            break;

        case motor_dir_a_forward:

            logs.duty[motor].a = duty_a;
            gpio_set_output_value(motor_map[motor].dir_a, true);
            break;

        case motor_dir_a_backward:

            logs.duty[motor].a = duty_a;
            gpio_set_output_value(motor_map[motor].dir_a, false);
            break;

        case motor_dir_b_forward:

            logs.duty[motor].b = duty_b;
            gpio_set_output_value(motor_map[motor].dir_b, true);
            break;

        case motor_dir_b_backward:

            logs.duty[motor].b = duty_b;
            gpio_set_output_value(motor_map[motor].dir_b, false);
            break;

        default:

            ESP_LOGE("motor_move", "Impossible direction selected: %d", direction);
            break;
    }

    // Then use log values to generate PWM
    pwm_generate(&motor_map[motor].config.pwm, PWM_A, logs.duty[motor].a, duty_type);
    pwm_generate(&motor_map[motor].config.pwm, PWM_B, logs.duty[motor].b, duty_type);
}

void motor_stop(motor_E motor)
{
    pwm_stop(&motor_map[motor].config.pwm, PWM_AB);
    logs.duty[motor].a = zero_duty;
    logs.duty[motor].b = zero_duty;
}

motor_logs_S * motor_get_logs(void)
{
    return &logs;
}
