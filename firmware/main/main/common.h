#pragma once
// Standard libraries
#include <stdint.h>
// FreeRTOS libraries
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
// Framework libraries
#include "string.h"
#include "errno.h"
#include "esp_log.h"
#include "esp_err.h"
#include "rom/ets_sys.h"
// Project settings
#include "config.h"
#include "credentials.h"    // Must include this to connect to wifi, @NOTE : create your own as it is gitignored



/**
 *  @module      : common
 *  @description : Common enums, defines, queues, and semaphores
 */

/// Divide tasks into three priorities
typedef enum
{
    PRIORITY_LOW     = 2,
    PRIORITY_MED     = 3,
    PRIORITY_HIGH    = 4,
    PRIORITY_HIGHEST = 5,
} rtos_priority_E;

// Task creation stack sizes
#define _512B   (512U)
#define _1KB    (1024U)
#define _2KB    (2048U)
#define _4KB    (4096U)
#define _8KB    (8192U)
#define _16KB   (16384U)

// Helper macros for size comparison or related
#define MAX(a, b)   ((a > b) ? (a) : (b))
#define MIN(a, b)   ((a < b) ? (a) : (b))
#define MAX_MIN_CLAMP(value, min, max) (MAX(MIN(value, max), min))

// Tick/time macros
#define TICK_MS(ms)         (ms / portTICK_PERIOD_MS)
#define DELAY_MS(ms)        (vTaskDelay(ms / portTICK_PERIOD_MS))
#define DELAY_US(us)        (ets_delay_us(us))
#define MAX_DELAY           (portMAX_DELAY)
#define NO_DELAY            (0)
#define ONE_MIN             (60U * 1000U)

// Event group bits
#define BIT_START           (1 << 0)
#define BIT_STOP            (1 << 1)
#define BIT_CONNECTED       (1 << 2)
#define BIT_DISCONNECTED    (1 << 3)
#define BIT_SERVER_READY    (1 << 4)
#define BIT_CLIENT_READY    (1 << 5)

// Global event group for status communication
extern EventGroupHandle_t StatusEventGroup;
