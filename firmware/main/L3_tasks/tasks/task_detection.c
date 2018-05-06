#include "tasks.h"
#include "infrared.h"
#include "motor.h"
#include "cmd_handler.h"



typedef struct
{
    uint8_t trip_value_cm;
    gpio_E gpio;
} sensor_context_S;

static const sensor_context_S sensor[infrared_max] =
{
    [infrared_bottom]    = { .trip_value_cm = 40, .gpio = gpio_adc_infrared_bottom    },
    [infrared_top_left]  = { .trip_value_cm = 50, .gpio = gpio_adc_infrared_top_left  },
    [infrared_top_right] = { .trip_value_cm = 50, .gpio = gpio_adc_infrared_top_right },
};

/// Maps IR sensors to gpio numbers
static const gpio_E gpios[infrared_max] = 
{
    [infrared_bottom]    = gpio_adc_infrared_bottom,
    [infrared_top_left]  = gpio_adc_infrared_top_left,
    [infrared_top_right] = gpio_adc_infrared_top_right,
};

static infrared_readings_S readings = { 0 };

static bool sensor_tripped[infrared_max] = { 0 };

static bool object_currently_blocking = false;

/**
 *  Initializes each sensor and checks if the sensor is operational
 *  @returns False if any of the sensors are not operational
 */
static bool infrared_self_test(void)
{
    bool success = true;
    bool sensor_functional[infrared_max] = { 0 };

    infrared_initialize(gpios, sensor_functional);

    for (uint8_t sensor = 0; sensor < infrared_max; sensor++)
    {
        if (!sensor_functional[sensor])
        {
            ESP_LOGE("task_detection", "Infrared %d self test failed", sensor);
            success = false;
        }

        DELAY_MS(100);
    }

    return success;
}

static void backup_slightly(void)
{
    motor_stop(motor_wheels);
    navigation_backup(20);
    DELAY_MS(1000);
    motor_stop(motor_wheels);
}

static void infrared_analyze_readings(void)
{
    /**
     *  Depending on which combination of sensors tripped, react accordingly
     *  Turns on override mode before controlling robot from firmware, then when finished
     *  deactivate override mode and let the server regain control
     *  @TODO : Remove repetition after confirmed working
     */

    if (sensor_tripped[infrared_top_left] || sensor_tripped[infrared_top_right] || sensor_tripped[infrared_bottom])
    {
#if TESTING
        const navigation_state_E current_state = navigation_get_state();
        if ((current_state == navigation_state_navigating_path) ||
            (current_state == navigation_state_reached_door)    ||
            (current_state == navigation_state_deliver_package))
        {
            ESP_LOGI("infrared_analyze_readings", "DELIVERING PACKAGE!");
            deliver_package();
        }
#else
        cmd_handler_set_override(true);
        {
            if (sensor_tripped[infrared_bottom])
            {
                ESP_LOGE("infrared_analyze_readings", "Bottom sensor detected! %f", readings.distance_cm[infrared_bottom]);
                backup_slightly();
            }
            else if (sensor_tripped[infrared_top_left] && !sensor_tripped[infrared_top_right])
            {
                ESP_LOGE("infrared_analyze_readings", "Left sensor detected! %f", readings.distance_cm[infrared_top_left]);
                backup_slightly();
                motor_move(motor_wheels , motor_dir_a_forward, 20);                 
                DELAY_MS(1000);
                motor_stop(motor_wheels);
            }
            else if (!sensor_tripped[infrared_top_left] && sensor_tripped[infrared_top_right])
            {
                ESP_LOGE("infrared_analyze_readings", "Right sensor detected! %f", readings.distance_cm[infrared_top_right]);
                backup_slightly();
                motor_move(motor_wheels , motor_dir_b_forward, 20);                 
                DELAY_MS(1000);
                motor_stop(motor_wheels);
            }
            else if (sensor_tripped[infrared_top_left] && sensor_tripped[infrared_top_right])
            {
                ESP_LOGE("infrared_analyze_readings", "Both sensors detected! %f %f", readings.distance_cm[infrared_top_left], readings.distance_cm[infrared_top_right]);
                backup_slightly();
            }
        }
        cmd_handler_set_override(false);
#endif
    }
}

void task_detection(task_param_T params)
{
    // Wait a second to make sure sensors are powered and have had some time to stabilize
    DELAY_MS(1000);

    const uint8_t num_samples = 5;
    const uint8_t max_retries = 60;
    const uint16_t delay_between_samples_ms = 40;
    const uint16_t delay_between_infrared_readings_ms = 250;

#if TESTING
    infrared_self_test();
#else
    bool operational = false;

    // While infrared sensors are failing self test, keep retrying
    for (uint8_t retry = 0; retry < max_retries; retry++)
    {
        if ((operational = infrared_self_test()))
        {
            break;
        }

        DELAY_MS(1000);
    }

    if (!operational)
    {
        ESP_LOGE("task_detection", "At least one sensor failed self test, suspending task...")
        vTaskSuspend(NULL);
    }
#endif

    // Main Loop
    while (1)
    {
        bool recently_detected_object = false;
    
        /**
         *  For each sensor check if it finds an object within range
         *  If any find an object, break the loop and immediately pause motors
         *  If it doesn't find anything, resume wheels
         */

        infrared_burst_sample_all(num_samples, delay_between_samples_ms, &readings);

        for (infrared_E ir = (infrared_E)0; ir < infrared_max; ir++)
        {
            sensor_tripped[ir] = (readings.distance_cm[ir] < sensor[ir].trip_value_cm);
        }

        infrared_analyze_readings();

        // ESP_LOGI("task_detection", "Average Sample : Bottom=%f | Left=%f | Right=%f", readings.distance_cm[infrared_bottom], 
        //                                                                               readings.distance_cm[infrared_top_left], 
        //                                                                               readings.distance_cm[infrared_top_right]);
        DELAY_MS(delay_between_infrared_readings_ms);
    }
}
