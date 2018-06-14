#include "navigation.h"

#include "motor.h"



/// @ { Can not call RTOS delay because it will context switch
#define MS_TO_US(ms)                          (ms            * 1000UL)
#define SEC_TO_MS(sec)                        (MS_TO_US(sec) * 1000UL)
#define DELAY_SEC_WITHOUT_CONTEXT_SWITCH(sec) (DELAY_US(SEC_TO_MS(sec)))
/// @ }

/// Current state of the state machine, updated from the server
static navigation_state_E current_navigation_state = navigation_state_navigating_sidewalk;

void navigation_backup(const float duty, const uint32_t delay_ms)
{
    motor_stop(motor_wheels);
    {
        motor_move(motor_wheels, motor_dir_both_backward, duty);
        DELAY_MS(delay_ms);
    }
    motor_stop(motor_wheels);
}

navigation_state_E navigation_get_state(void)
{
    return current_navigation_state;
}

void navigation_set_state(const navigation_state_E new_state)
{
    if (new_state < navigation_state_last_invalid)
    {
        current_navigation_state = new_state;
    }
}

void navigation_pivot_left(void)
{
    const float duty_to_pivot_90_degrees = 37.0f;
    const uint8_t time_to_pivot_90_degrees = 3;

    motor_move(motor_wheels, motor_dir_pivot_left, duty_to_pivot_90_degrees);
    DELAY_SEC_WITHOUT_CONTEXT_SWITCH(time_to_pivot_90_degrees);
    motor_stop(motor_wheels);
}

void navigation_pivot_right(void)
{
    const float duty_to_pivot_90_degrees = 37.0f;
    const uint8_t time_to_pivot_90_degrees = 3;

    motor_move(motor_wheels, motor_dir_pivot_right, duty_to_pivot_90_degrees);
    DELAY_SEC_WITHOUT_CONTEXT_SWITCH(time_to_pivot_90_degrees);
    motor_stop(motor_wheels);
}
