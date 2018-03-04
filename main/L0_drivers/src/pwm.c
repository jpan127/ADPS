#include "pwm.h"



void pwm_init(const pwm_config_S * config)
{
    // Set the PWM IO signals
    mcpwm_io_signals_t io_a = MCPWM0A;
    mcpwm_io_signals_t io_b = MCPWM0B;

    switch (config->pwm.timer)
    {
        case MCPWM_TIMER_0: io_a = MCPWM0A; io_b = MCPWM0B; break;
        case MCPWM_TIMER_1: io_a = MCPWM1A; io_b = MCPWM1B; break;
        case MCPWM_TIMER_2: io_a = MCPWM2A; io_b = MCPWM2B; break;
        default:                                            return;
    }

    // Initialize PWM A if it is populated
    if (GPIO_NOT_USING != gpio_get_pin_number(config->pwm_a))
    {
        ESP_ERROR_CHECK(mcpwm_gpio_init(config->pwm.unit, io_a, gpio_get_pin_number(config->pwm_a)));
    }

    // Initialize PWM B if it is populated
    if (GPIO_NOT_USING != gpio_get_pin_number(config->pwm_b))
    {
        ESP_ERROR_CHECK(mcpwm_gpio_init(config->pwm.unit, io_b, gpio_get_pin_number(config->pwm_b)))        
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
    ESP_ERROR_CHECK(mcpwm_init(config->pwm.unit, config->pwm.timer, &mcpwm_config));
    ESP_LOGI("pwm_init", "Initialized PWM Configuration.");
}

void pwm_start(pwm_S * pwm_pair)
{
    ESP_ERROR_CHECK(mcpwm_start(pwm_pair->unit, pwm_pair->timer));
}

void pwm_generate(pwm_S * pwm_pair, pwm_E pwm, pwm_duty_U duty, pwm_duty_type_E duty_type)
{
    // Make sure duty is less than 100
    duty.percent = MIN(100.0f, duty.percent);
    
    switch (pwm)
    {
        // Only for MCPWMXA
        case PWM_A:

            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A));
            if (pwm_duty_percent == duty_type)
            {
                ESP_ERROR_CHECK(mcpwm_set_duty(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A, duty.percent));
            }
            else
            {
                ESP_ERROR_CHECK(mcpwm_set_duty_in_us(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A, duty.us));
            }
            // ESP_LOGI("pwm_generate", "Generating MCPWMXA : %f or %u", duty.percent, duty.us);
            ESP_ERROR_CHECK(mcpwm_set_duty_type(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A, MCPWM_DUTY_MODE_0));
            break;

        // Only for MCPWMXB
        case PWM_B:

            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B));
            if (pwm_duty_percent == duty_type)
            {
                ESP_ERROR_CHECK(mcpwm_set_duty(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B, duty.percent));
            }
            else
            {
                ESP_ERROR_CHECK(mcpwm_set_duty_in_us(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B, duty.us));
            }
            // ESP_LOGI("pwm_generate", "Generating MCPWMXB : %f or %u", duty.percent, duty.us);
            ESP_ERROR_CHECK(mcpwm_set_duty_type(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B, MCPWM_DUTY_MODE_0));
            break;

        // Both MCPWMXA and MCPWMXB
        case PWM_AB:

            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A));
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B));
            if (pwm_duty_percent == duty_type)
            {
                ESP_ERROR_CHECK(mcpwm_set_duty(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A, duty.percent));
                ESP_ERROR_CHECK(mcpwm_set_duty(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B, duty.percent));
                printf("Setting duty %f%%\n", duty.percent);
            }
            else
            {
                ESP_ERROR_CHECK(mcpwm_set_duty_in_us(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A, duty.us));
                ESP_ERROR_CHECK(mcpwm_set_duty_in_us(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B, duty.us));
            }
            ESP_ERROR_CHECK(mcpwm_set_duty_type(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A, MCPWM_DUTY_MODE_0));
            ESP_ERROR_CHECK(mcpwm_set_duty_type(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B, MCPWM_DUTY_MODE_0));
            break;

        default:

            ESP_LOGE("pwm_generate", "Undefined pwm selected");
            break;
    }

    if (pwm_duty_percent == duty_type)
    {
        ESP_LOGI("pwm_generate", "Duty: %f %f", mcpwm_get_duty(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A),
                                                mcpwm_get_duty(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B));
    }
    else
    {
        ESP_LOGI("pwm_generate", "Duty: %uus", duty.us);     
    }
}

void pwm_stop(pwm_S * pwm_pair, pwm_E pwm)
{
    switch (pwm)
    {
        case PWM_A:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A));
            ESP_LOGI("pwm_stop", "Stopping MCPWMXA.");
            break;
        case PWM_B:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B));
            ESP_LOGI("pwm_stop", "Stopping MCPWMXB.");
            break;
        case PWM_AB:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_A));
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_pair->unit, pwm_pair->timer, MCPWM_OPR_B));
            ESP_LOGI("pwm_stop", "Stopping MCPWMXA + MCPWMXB.");
            break;
        default:
            break;
    }
}