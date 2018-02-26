#include "pwm.h"
#include "gpio.h"



void pwm_init(const pwm_config_S * config)
{
    // Set the PWM IO signals
    mcpwm_io_signals_t io_a = MCPWM0A;
    mcpwm_io_signals_t io_b = MCPWM0B;

    switch (config->timer)
    {
        case MCPWM_TIMER_0: io_a = MCPWM0A; io_b = MCPWM0B; break;
        case MCPWM_TIMER_1: io_a = MCPWM1A; io_b = MCPWM1B; break;
        case MCPWM_TIMER_2: io_a = MCPWM2A; io_b = MCPWM2B; break;
        default:                                            return;
    }

    // Initialize GPIO A if it is populated
    if (GPIO_NOT_USING != config->gpio_a)
    {
        ESP_ERROR_CHECK(mcpwm_gpio_init(config->pwm_unit, io_a, config->gpio_a));
    }

    // Initialize GPIO B if it is populated
    if (GPIO_NOT_USING != config->gpio_b)
    {
        ESP_ERROR_CHECK(mcpwm_gpio_init(config->pwm_unit, io_b, config->gpio_b))        
    }

    const mcpwm_config_t mcpwm_config =
    {
        .frequency    = config->frequency,  ///< Frequency of PWM signal
        .cmpr_a       = 0,                  ///< Start off with no duty cycle
        .cmpr_b       = 0,                  ///< Start off with no duty cycle
        .duty_mode    = MCPWM_DUTY_MODE_0,  ///< Active high duty
        .counter_mode = MCPWM_UP_COUNTER,   ///< Counts up
    };

    // Initialize PWM
    ESP_ERROR_CHECK(mcpwm_init(config->pwm_unit, config->timer, &mcpwm_config));
    ESP_LOGI("pwm_init", "Initialized PWM Configuration.");
}

void pwm_start(pwm_S * pwm_pair)
{
    ESP_ERROR_CHECK(mcpwm_start(pwm_pair->pwm_unit, pwm_pair->timer));
}

void pwm_generate(pwm_S * pwm_pair, pwm_E pwm, pwm_duty_U duty, pwm_duty_type_E duty_type)
{
    // Make sure duty is less than 100
    duty.percent = MIN(100.0f, duty.percent);
    
    switch (pwm)
    {
        // Only for MCPWMXA
        case PWM_A:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_A));
            if (pwm_duty_percent == duty_type)
            {
                ESP_ERROR_CHECK(mcpwm_set_duty(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_A, duty.percent));
            }
            else
            {
                ESP_ERROR_CHECK(mcpwm_set_duty_in_us(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_A, duty.us));
            }
            ESP_LOGI("pwm_generate", "Generating MCPWMXA : %f or %u", duty.percent, duty.us);
            break;
        // Only for MCPWMXB
        case PWM_B:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_B));
            if (pwm_duty_percent == duty_type)
            {
                ESP_ERROR_CHECK(mcpwm_set_duty(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_B, duty.percent));
            }
            else
            {
                ESP_ERROR_CHECK(mcpwm_set_duty_in_us(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_B, duty.us));
            }
            ESP_LOGI("pwm_generate", "Generating MCPWMXB : %f or %u", duty.percent, duty.us);
            break;
        // Both MCPWMXA and MCPWMXB
        case PWM_AB:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_A));
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_B));
            if (pwm_duty_percent == duty_type)
            {
                ESP_ERROR_CHECK(mcpwm_set_duty(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_A, duty.percent));
                ESP_ERROR_CHECK(mcpwm_set_duty(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_B, duty.percent));
            }
            else
            {
                ESP_ERROR_CHECK(mcpwm_set_duty_in_us(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_A, duty.us));
                ESP_ERROR_CHECK(mcpwm_set_duty_in_us(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_B, duty.us));
            }
            ESP_LOGI("pwm_generate", "Generating MCPWMXA + MCPWMXB : %f or %u", duty.percent, duty.us);
            break;
        default:
            break;
    }
}

void pwm_stop(pwm_S * pwm_pair, pwm_E pwm)
{
    switch (pwm)
    {
        case PWM_A:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_A));
            ESP_LOGI("pwm_stop", "Stopping MCPWMXA.");
            break;
        case PWM_B:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_B));
            ESP_LOGI("pwm_stop", "Stopping MCPWMXB.");
            break;
        case PWM_AB:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_A));
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->pwm_unit, pwm_pair->timer, MCPWM_OPR_B));
            ESP_LOGI("pwm_stop", "Stopping MCPWMXA + MCPWMXB.");
            break;
        default:
            break;
    }
}