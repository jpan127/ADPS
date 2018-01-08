#include "tasks.h"
// Project libraries
#include "motor.h"

#define ENA      ((gpio_num_t)17)
#define ENB      ((gpio_num_t)16)
#define PWMA     ((gpio_num_t)4)
#define PWMB     ((gpio_num_t)0)
#define PWMUNIT  (MCPWM_UNIT_0)
#define PWMTIMER (MCPWM_TIMER_0)

static motor_S motor_wheels;

void Init_NavigationTask(void)
{
    motor_wheels.en_a     = ENA;
    motor_wheels.en_b     = ENB;
    motor_wheels.pwm_a    = PWMA;
    motor_wheels.pwm_b    = PWMB;
    motor_wheels.pwm_unit = PWMUNIT;
    motor_wheels.timer    = PWMTIMER;

    motor_init(MOTOR_0, &motor_wheels);
}

void NavigationTask(void *p)
{    
    // Main loop
    while (1)
    {
        ESP_LOGI("NavigationTask", "Forward.");
        motor_move(MOTOR_0, MOTOR_DIRECTION_FORWARD, (float)100);
        DELAY_MS(5000);

        ESP_LOGI("NavigationTask", "Backward.");
        motor_move(MOTOR_0, MOTOR_DIRECTION_BACKWARD, (float)50);
        DELAY_MS(5000);

        ESP_LOGI("NavigationTask", "Stop.");
        motor_move(MOTOR_0, MOTOR_DIRECTION_STOP, (float)0);
        DELAY_MS(5000);
    }

    vTaskDelete(NULL);
}