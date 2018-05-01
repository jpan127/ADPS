#include "motor.h"
// Project libraries
#include "packet.h"
#include "gpio.h"



#define MIN_DUTY_RATIO_US  (500)
#define MAX_DUTY_RATIO_US  (2500)

/// Map of motor structs
static motor_config_S motor_map[motor_max] = { 0 };

/// Motor logs
static motor_logs_S logs = { 0 };

/// A duty union of zero
static const pwm_duty_U zero_duty = { 0 };

/// Duty constants
static const uint16_t duty_ratio_range_us = MAX_DUTY_RATIO_US - MIN_DUTY_RATIO_US;
static const uint8_t max_degree           = 180;
static const float max_duty_percent       = 100.0f;

/// Flag to say pause the state of the motors, and restore context later
static bool pause_wheel_motors = false;

/// Stores the previous duties of the wheels prior to pausing
static float previous_wheel_motor_duties[2] = { 0 };

/// @TODO : Fix this
static const bool motor_polarity_forward = true;
static const bool motor_polarity_backward = false;

/**
 *  Converts a servo degree from a range of [0, max_degree] to a PWM pulse width in microseconds
 *  @param degree : The servo degree to change to
 */
static uint16_t servo_degree_to_pulse_width(int16_t degree)
{
    // Limit to max_degree
    degree = MAX(0, degree);
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

            gpio_set_output_value(motor_map[motor].dir_a, motor_polarity_backward);
            gpio_set_output_value(motor_map[motor].dir_b, motor_polarity_backward);
            pwm_generate(&motor_map[motor].config.pwm, PWM_AB, pwm_duty, duty_type);
            logs.duty[motor].a = duty;
            logs.duty[motor].b = duty;
            break;

        case motor_dir_both_backward:

            gpio_set_output_value(motor_map[motor].dir_a, motor_polarity_forward);
            gpio_set_output_value(motor_map[motor].dir_b, motor_polarity_forward);
            pwm_generate(&motor_map[motor].config.pwm, PWM_AB, pwm_duty, duty_type);
            logs.duty[motor].a = duty;
            logs.duty[motor].b = duty;
            break;

        case motor_dir_a_forward:

            gpio_set_output_value(motor_map[motor].dir_a, motor_polarity_backward);
            pwm_generate(&motor_map[motor].config.pwm, PWM_A, pwm_duty, duty_type);
            logs.duty[motor].a = duty;
            break;

        case motor_dir_a_backward:

            gpio_set_output_value(motor_map[motor].dir_a, motor_polarity_forward);
            pwm_generate(&motor_map[motor].config.pwm, PWM_A, pwm_duty, duty_type);
            logs.duty[motor].a = duty;
            break;

        case motor_dir_b_forward:

            gpio_set_output_value(motor_map[motor].dir_b, motor_polarity_backward);
            pwm_generate(&motor_map[motor].config.pwm, PWM_B, pwm_duty, duty_type);
            logs.duty[motor].b = duty;
            break;

        case motor_dir_b_backward:

            gpio_set_output_value(motor_map[motor].dir_b, motor_polarity_forward);
            pwm_generate(&motor_map[motor].config.pwm, PWM_B, pwm_duty, duty_type);
            logs.duty[motor].b = duty;
            break;

        default:

            ESP_LOGE("motor_move", "Impossible direction selected: %d", direction);
            break;
    }

    // if (pwm_duty_percent == duty_type)
    // {
    //     ESP_LOGI("motor_move", "Duty : %f %f%%", duty, percent.percent);
    // }
    // else
    // {
    //     ESP_LOGI("motor_move", "Duty : %f %uus", duty, us.us);
    // }
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

    // Step the duty percentage
    float adjusted_duty_a = (duty_increment == adjust_type) ? (logs.duty[motor].a + step) : (logs.duty[motor].a - step);
    float adjusted_duty_b = (duty_increment == adjust_type) ? (logs.duty[motor].b + step) : (logs.duty[motor].b - step);

    bool adjust_left  = false;
    bool adjust_right = false;

    // Make sure duties are positive
    adjusted_duty_a = MAX(0.0f, adjusted_duty_a);
    adjusted_duty_b = MAX(0.0f, adjusted_duty_b);

    // Make sure duties are within upper bound
    adjusted_duty_a = MIN(max_duty_percent, adjusted_duty_a);
    adjusted_duty_b = MIN(max_duty_percent, adjusted_duty_b);
    
    // Set the appropriate union member depending on which motor it is
    if (pwm_duty_percent == duty_type)
    {
        duty_a.percent = adjusted_duty_a;
        duty_b.percent = adjusted_duty_b;
    }
    else
    {
        duty_a.us = servo_degree_to_pulse_width(adjusted_duty_a);
        duty_b.us = servo_degree_to_pulse_width(adjusted_duty_b);
    }

    // Set the appropriate GPIO enables and store into log
    switch (direction)
    {
        case motor_dir_both_forward:
        {
            gpio_set_output_value(motor_map[motor].dir_b, motor_polarity_backward);
            gpio_set_output_value(motor_map[motor].dir_b, motor_polarity_backward);
            logs.duty[motor].a = adjusted_duty_a;
            logs.duty[motor].b = adjusted_duty_b;
            adjust_left  = true;
            adjust_right = true;
            break;
        }
        case motor_dir_both_backward:
        {
            gpio_set_output_value(motor_map[motor].dir_a, motor_polarity_forward);
            gpio_set_output_value(motor_map[motor].dir_b, motor_polarity_forward);
            logs.duty[motor].a = adjusted_duty_a;
            logs.duty[motor].b = adjusted_duty_b;
            adjust_left  = true;
            adjust_right = true;
            break;
        }
        case motor_dir_left:
        {
            gpio_set_output_value(motor_map[motor].dir_a, motor_polarity_forward);
            logs.duty[motor].a = adjusted_duty_a;
            adjust_left  = true;
            adjust_right = false;
            break;
        }
        case motor_dir_right:
        {
            gpio_set_output_value(motor_map[motor].dir_b, motor_polarity_forward);
            logs.duty[motor].b = adjusted_duty_b;
            adjust_left  = false;
            adjust_right = true;
            break;
        }
        // case motor_dir_a_forward:

        //     logs.duty[motor].a = adjusted_duty_a;
        //     adjust_left  = true;
        //     adjust_right = false;
        //     break;

        // case motor_dir_a_backward:

        //     logs.duty[motor].a = adjusted_duty_a;
        //     adjust_left  = true;
        //     adjust_right = false;
        //     break;

        // case motor_dir_b_forward:

        //     logs.duty[motor].b = adjusted_duty_b;
        //     adjust_left  = false;
        //     adjust_right = true;
        //     break;

        // case motor_dir_b_backward:

        //     logs.duty[motor].b = adjusted_duty_b;
        //     adjust_left  = false;
        //     adjust_right = true;
        //     break;

        default:
        {
            ESP_LOGE("motor_move", "Impossible direction selected: %d", direction);
            break;
        }
    }

    // Generate the adjusted PWM signals
    if (adjust_left)  { pwm_generate(&motor_map[motor].config.pwm, PWM_A, duty_a, duty_type); }
    if (adjust_right) { pwm_generate(&motor_map[motor].config.pwm, PWM_B, duty_b, duty_type); }
}

void motor_stop(motor_E motor)
{
    pwm_stop(&motor_map[motor].config.pwm, PWM_AB);
    logs.duty[motor].a = 0;
    logs.duty[motor].b = 0;
}

bool motor_are_wheels_paused(void)
{
    return pause_wheel_motors;
}

void motor_pause_wheels(void)
{
    static const motor_E motor_to_pause = motor_wheels;

    previous_wheel_motor_duties[0] = logs.duty[motor_to_pause].a;
    previous_wheel_motor_duties[1] = logs.duty[motor_to_pause].b;

    motor_stop(motor_to_pause);

    pause_wheel_motors = true;
}

void motor_resume_wheels(void)
{
    static const motor_E motor_to_resume = motor_wheels;

    motor_move(motor_to_resume, motor_dir_left , previous_wheel_motor_duties[0]);
    motor_move(motor_to_resume, motor_dir_right, previous_wheel_motor_duties[1]);

    pause_wheel_motors = false;
}

motor_logs_S * motor_get_logs(void)
{
    return &logs;
}
