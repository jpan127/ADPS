#include "tasks.h"
#include "infrared.h"


void task_detection(task_param_T params)
{
    const gpio_E gpio = (const gpio_E)(params);
    const bool sensor_functional = infrared_initialize(gpio);

    if (!sensor_functional)
    {
        ESP_LOGE("task_detection", "Infrared self test failed, suspending this task");
        vTaskSuspend(NULL);
    }

    const uint8_t samples   = 5;
    const uint16_t delay_us = 100;

    // Main Loop
    while (1)
    {
        const uint32_t average_sample = infrared_burst_sample(gpio, samples, delay_us);
        ESP_LOGI("task_detection", "Average Sample : %d", average_sample);
        DELAY_MS(1000);
    }
}
