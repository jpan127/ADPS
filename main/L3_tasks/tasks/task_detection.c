#include "tasks.h"
#include "infrared.h"


void task_detection(task_param_T params)
{
    const gpio_E gpios[infrared_max] = 
    {
        gpio_adc_infrared_bottom,
        gpio_adc_infrared_top_left,
        gpio_adc_infrared_top_right,
    };

    bool sensor_functional[infrared_max] = { 0 };

    infrared_initialize(gpios, sensor_functional);

    for (uint8_t sensor = 0; sensor < infrared_max; sensor++)
    {
        if (!sensor_functional[sensor])
        {
            ESP_LOGE("task_detection", "Infrared %d self test failed, suspending this task", sensor);
            // vTaskSuspend(NULL); // @TODO : Change back
        }
    }

    const uint8_t samples   = 5;
    const uint16_t delay_us = 100;

    // Main Loop
    while (1)
    {
        for (uint8_t sensor = 0; sensor < infrared_max; sensor++)
        {
            const uint32_t average_sample = infrared_burst_sample(gpios[sensor], samples, delay_us);
            ESP_LOGI("task_detection", "Average Sample : %d", average_sample);
            DELAY_MS(250);
        }
    }
}
