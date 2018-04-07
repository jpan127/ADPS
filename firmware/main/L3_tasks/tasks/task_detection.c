#include "tasks.h"
#include "infrared.h"


static const gpio_E gpios[infrared_max] = 
{
    [infrared_1] = gpio_adc_infrared_bottom,
    [infrared_2] = gpio_adc_infrared_top_left,
    [infrared_3] = gpio_adc_infrared_top_right,
};

static bool infrared_self_test(void)
{
    bool success = true;
    bool sensor_functional[infrared_max] = { 0 };

    infrared_initialize(gpios, sensor_functional);

    for (uint8_t sensor = 0; sensor < infrared_max; sensor++)
    {
        if (!sensor_functional[sensor])
        {
            ESP_LOGE("task_detection", "Infrared %d self test failed, suspending this task", sensor);
            success = false;
        }
    }

    return success;
}

void task_detection(task_param_T params)
{
    // Wait a second to make sure sensors are powered and have had some time to stabilize
    DELAY_MS(1000);

    const uint8_t samples   = 5;
    const uint16_t delay_us = 100;
    const uint8_t max_retries = 5;

    bool operational = false;

    for (uint8_t retry = 0; retry < max_retries; retry++)
    {
        if ((operational = infrared_self_test()))
        {
            break;
        }
    }

    if (!operational)
    {
        // @TODO : Change back
        // vTaskSuspend(NULL);
    }

    // Main Loop
    while (1)
    {
        for (infrared_E ir = (infrared_E)0; ir < infrared_max; ir++)
        {
            const uint32_t average_sample = infrared_burst_sample(ir, samples, delay_us);
            ESP_LOGI("task_detection", "Average Sample : %d", average_sample);
            DELAY_MS(250);
        }
    }
}
