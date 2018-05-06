#include "navigation.h"

#include "motor.h"



/// Current state of the state machine, updated from the server
static navigation_state_E current_navigation_state = navigation_state_navigating_sidewalk;

void navigation_deliver_package(void)
{
    static const uint16_t time_to_drop_package_ms = 7000;
    motor_move(motor_delivery, motor_dir_delivery_forward, 100.0f);
    DELAY_MS(time_to_drop_package_ms);
    motor_stop(motor_delivery);    
}

void navigation_backup(float duty)
{
    motor_stop(motor_wheels);
    motor_move(motor_wheels, motor_dir_both_backward, duty);
}

navigation_state_E navigation_get_state(void)
{
    return current_navigation_state;
}

void navigation_set_state(const navigation_state_E new_state)
{
    current_navigation_state = new_state;
}

void navigation_pivot_left(void)
{
    motor_move(motor_wheels, motor_dir_pivot_left, 40.0f);
    motor_stop(motor_wheels);
    DELAY_MS(1000);
}

void navigation_pivot_right(void)
{
    motor_move(motor_wheels, motor_dir_pivot_right, 40.0f);
    motor_stop(motor_wheels);
    DELAY_MS(1000);
}
