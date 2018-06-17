#include "tasks.h"
#include "infrared.h"
#include "motor.h"
#include "cmd_handler.h"



/// Context information about an IR sensor
typedef struct
{
    const uint8_t trip_value_cm;  ///< The maximum trip value before the system decides an object is too close
    const gpio_E gpio;            ///< The GPIO enumeration of the sensor
    uint32_t trip_count;
} sensor_context_S;

static const uint8_t trigger_trip_count = 2;

/// Contexts for all IR sensors
static sensor_context_S sensor[infrared_max] =
{
    [infrared_bottom]    = { .trip_value_cm = 40, .gpio = gpio_adc_infrared_bottom    , .trip_count = 0 },
    [infrared_top_left]  = { .trip_value_cm = 80, .gpio = gpio_adc_infrared_top_left  , .trip_count = 0 },
    [infrared_top_right] = { .trip_value_cm = 80, .gpio = gpio_adc_infrared_top_right , .trip_count = 0 },
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

/**
 *  Initializes each sensor and checks if the sensor is operational
 *  @returns False if any of the sensors are not operational
 */
static bool infrared_self_test(void)
{
    const gpio_E gpios[infrared_max] =
    {
        [infrared_bottom]    = sensor[infrared_bottom].gpio,
        [infrared_top_left]  = sensor[infrared_top_left].gpio,
        [infrared_top_right] = sensor[infrared_top_right].gpio,
    };

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

/**
 *  Interprets the most current IR sensor readings
 *  Acts appropriately, depending on which sensors triggered
 */
static void infrared_analyze_readings(void)
{
    /**
     *  Depending on which combination of sensors tripped, react accordingly
     *  Turns on override mode before controlling robot from firmware, then when finished
     *  deactivate override mode and let the server regain control
     *  @TODO : Remove repetition after confirmed working
     */

    static bool delivered = false;

    if (sensor_tripped[infrared_top_left] || sensor_tripped[infrared_top_right] || sensor_tripped[infrared_bottom])
    {
        if (!delivered)
        {
            const navigation_state_E current_state = navigation_get_state();
            if ((current_state == navigation_state_navigating_path) ||
                (current_state == navigation_state_reached_door)    ||
                (current_state == navigation_state_deliver_package))
            {
                DISABLE_EXTERNAL_COMMANDS();
                {
                    ESP_LOGI("infrared_analyze_readings", "DELIVERING PACKAGE!");
                    motor_stop(motor_wheels);
                    motor_move(motor_delivery, motor_dir_delivery_forward, 100.0f);
                    DELAY_MS(7000);
                    motor_stop(motor_delivery);
                    delivered = true;
                }
                // ENABLE_EXTERNAL_COMMANDS();
            }
            // sensor_tripped[infrared_top_left]  = false;
            // sensor_tripped[infrared_top_right] = false;
            // sensor_tripped[infrared_bottom]    = false;
        }
        sensor[infrared_top_left].trip_count  = 0;
        sensor[infrared_top_right].trip_count = 0;
        sensor[infrared_bottom].trip_count    = 0;
            // else
            // {
            //     if (sensor_tripped[infrared_bottom])
            //     {
            //         ESP_LOGE("infrared_analyze_readings", "Bottom sensor detected! %f", readings.distance_cm[infrared_bottom]);
            //         navigation_backup(20.0f, 1000);
            //         sensor_tripped[infrared_bottom] = false;
            //         sensor[infrared_bottom].trip_count = 0;
            //     }
            //     else if (sensor_tripped[infrared_top_left] && !sensor_tripped[infrared_top_right])
            //     {
            //         ESP_LOGE("infrared_analyze_readings", "Left sensor detected! %f", readings.distance_cm[infrared_top_left]);
            //         navigation_backup(20.0f, 1000);
            //         motor_move(motor_wheels , motor_dir_a_forward, 20);
            //         DELAY_MS(1000);
            //         motor_stop(motor_wheels);
            //         sensor_tripped[infrared_top_left] = false;
            //         sensor[infrared_top_left].trip_count = 0;
            //     }
            //     else if (!sensor_tripped[infrared_top_left] && sensor_tripped[infrared_top_right])
            //     {
            //         ESP_LOGE("infrared_analyze_readings", "Right sensor detected! %f", readings.distance_cm[infrared_top_right]);
            //         navigation_backup(20.0f, 1000);
            //         motor_move(motor_wheels , motor_dir_b_forward, 20);
            //         DELAY_MS(1000);
            //         motor_stop(motor_wheels);
            //         sensor_tripped[infrared_top_right] = false;
            //         sensor[infrared_top_right].trip_count = 0;
            //     }
            //     else if (sensor_tripped[infrared_top_left] && sensor_tripped[infrared_top_right])
            //     {
            //         ESP_LOGE("infrared_analyze_readings", "Both sensors detected! %f %f", readings.distance_cm[infrared_top_left], readings.distance_cm[infrared_top_right]);
            //         navigation_backup(20.0f, 1000);
            //         sensor_tripped[infrared_top_left]  = false;
            //         sensor_tripped[infrared_top_right] = false;
            //         sensor[infrared_top_left].trip_count = 0;
            //         sensor[infrared_top_right].trip_count = 0;
            //     }
            // }
    }
}

void task_detection(task_param_T params)
{
    // Wait a second to make sure sensors are powered and have had some time to stabilize @TODO: Remove because self test will have ample time
    DELAY_MS(1000);

    const uint8_t num_samples = 5;
    const uint8_t max_retries = 60;
    const uint16_t delay_between_samples_ms = 40;
    const uint16_t delay_between_infrared_readings_ms = 50;

#if TESTING
    infrared_self_test();
#else
    // Flag that is only false if all sensors fail self test
    bool operational = false;

    // While infrared sensors are failing self test, keep retrying
    for (uint8_t retry = 0; retry < max_retries; retry++)
    {
        if ((operational |= infrared_self_test()))
        {
            break;
        }

        DELAY_MS(1000);
    }

    if (!operational)
    {
        ESP_LOGE("task_detection", "All infrared sensors failed self test, suspending task...")
        vTaskSuspend(NULL);
    }
    else
    {
        ESP_LOGI("task_detection", "Task initialized and starting...");
    }
#endif

    // Main Loop
    while (1)
    {
        /**
         *  For each sensor check if it finds an object within range
         *  Depending on which combination of sensors tripped, react accordingly
         */

        infrared_burst_sample_all(num_samples, delay_between_samples_ms, &readings);

        for (infrared_E ir = (infrared_E)0; ir < infrared_max; ir++)
        {
            // Object too close, increment trip count
            if (readings.distance_cm[ir] < sensor[ir].trip_value_cm)
            {
                sensor_tripped[ir] = (++sensor[ir].trip_count >= trigger_trip_count);
            }
        }

        infrared_analyze_readings();

#if 1 /// EXTRA_DEBUG_MSGS
        ESP_LOGI("task_detection", "Average Sample : Bottom=%f | Left=%f | Right=%f", readings.distance_cm[infrared_bottom],
                                                                                      readings.distance_cm[infrared_top_left],
                                                                                      readings.distance_cm[infrared_top_right]);
#endif
        DELAY_MS(delay_between_infrared_readings_ms);
    }
}
