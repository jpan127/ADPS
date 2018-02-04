#include "pwm.h"
#include "gpio.h"



void pwm_init(mcpwm_unit_t pwm_unit, gpio_num_t gpio_a, gpio_num_t gpio_b, mcpwm_timer_t timer)
{
    // Set the PWM IO signals
    mcpwm_io_signals_t io_a = MCPWM0A;
    mcpwm_io_signals_t io_b = MCPWM0B;

    switch (timer)
    {
        case MCPWM_TIMER_0: io_a = MCPWM0A; io_b = MCPWM0B; break;
        case MCPWM_TIMER_1: io_a = MCPWM1A; io_b = MCPWM1B; break;
        case MCPWM_TIMER_2: io_a = MCPWM2A; io_b = MCPWM2B; break;
        default:                                            return;
    }

    // Initialize GPIO
    ESP_ERROR_CHECK(mcpwm_gpio_init(pwm_unit, io_a, gpio_a));
    // If gpio_b is GPIO_NUM_MAX, meaning only need gpio_a, dont use
    if (gpio_b < GPIO_NUM_MAX)
    {
        ESP_ERROR_CHECK(mcpwm_gpio_init(pwm_unit, io_b, gpio_b))        
    }

    const mcpwm_config_t config =
    {
        .frequency    = 1000,
        .cmpr_a       = 0,
        .cmpr_b       = 0,
        .duty_mode    = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER,
    };

    // Initialize PWM
    ESP_ERROR_CHECK(mcpwm_init(pwm_unit, timer, &config));
    ESP_LOGI("pwm_init", "Initialized PWM Configuration.");
}

void pwm_start(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer)
{
    ESP_ERROR_CHECK(mcpwm_start(pwm_unit, timer));
}

void pwm_generate(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer, pwm_E pwm, float duty)
{
    // Make sure duty is less than 100
    duty = MIN(100.0f, duty);
    
    switch (pwm)
    {
        // Only for MCPWMXA
        case PWM_A:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, timer, MCPWM_OPR_A));
            ESP_ERROR_CHECK(mcpwm_set_duty(pwm_unit, timer, MCPWM_OPR_A, duty));
            ESP_LOGI("pwm_generate", "Generating MCPWMXA : %f", duty);
            break;
        // Only for MCPWMXB
        case PWM_B:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, timer, MCPWM_OPR_B));
            ESP_ERROR_CHECK(mcpwm_set_duty(pwm_unit, timer, MCPWM_OPR_B, duty));
            ESP_LOGI("pwm_generate", "Generating MCPWMXB : %f", duty);
            break;
        // Both MCPWMXA and MCPWMXB
        case PWM_AB:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, timer, MCPWM_OPR_A));
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, timer, MCPWM_OPR_B));
            ESP_ERROR_CHECK(mcpwm_set_duty(pwm_unit, timer, MCPWM_OPR_A, duty));
            ESP_ERROR_CHECK(mcpwm_set_duty(pwm_unit, timer, MCPWM_OPR_B, duty));
            ESP_LOGI("pwm_generate", "Generating MCPWMXA + MCPWMXB : %f", duty);
            break;
    }
}

void pwm_stop(mcpwm_unit_t pwm_unit, mcpwm_timer_t timer, pwm_E pwm)
{
    switch (pwm)
    {
        case PWM_A:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, timer, MCPWM_OPR_A));
            ESP_LOGI("pwm_stop", "Stopping MCPWMXA.");
            break;
        case PWM_B:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, timer, MCPWM_OPR_B));
            ESP_LOGI("pwm_stop", "Stopping MCPWMXB.");
            break;
        case PWM_AB:
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, timer, MCPWM_OPR_A));
            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, timer, MCPWM_OPR_B));
            ESP_LOGI("pwm_stop", "Stopping MCPWMXA + MCPWMXB.");
            break;
    }
}