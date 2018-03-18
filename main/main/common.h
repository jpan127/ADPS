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
// Project libraries
#include "packet.h"



/**
 *  @module      : common
 *  @description : Common enums, defines, queues, and semaphores
 */

/// Divide tasks into three priorities
typedef enum
{
    PRIORITY_LOW  = 5,
    PRIORITY_MED  = 6,
    PRIORITY_HIGH = 7,
} rtos_priority_E;

// Task creation stack sizes
#define _512B   (128)
#define _1KB    (256)
#define _2KB    (512)
#define _4KB    (1024)
#define _8KB    (2048)
#define _16KB   (4096)
#define _32KB   (8192)
#define _64KB   (16384)

// Helper macros for size comparison or related
#define MAX(a, b)   ((a > b) ? (a) : (b))
#define MIN(a, b)   ((a < b) ? (a) : (b))

// Tick/time macros
#define TICK_MS(ms)         (ms / portTICK_PERIOD_MS)
#define DELAY_MS(ms)        (vTaskDelay(ms / portTICK_PERIOD_MS))
#define DELAY_US(us)        (ets_delay_us(us))
#define MAX_DELAY           (portMAX_DELAY)
#define NO_DELAY            (0)
#define ONE_MIN             (60*1000)

// Event group bits
#define BIT_START           (1 << 0)
#define BIT_STOP            (1 << 1)
#define BIT_CONNECTED       (1 << 2)
#define BIT_DISCONNECTED    (1 << 3)
#define BIT_SERVER_READY    (1 << 4)
#define BIT_CLIENT_READY    (1 << 5)

// Maximum size of recv buffer
#define MAX_BUFFER_SIZE     (1024)

// Global event group for status communication
extern EventGroupHandle_t StatusEventGroup;
