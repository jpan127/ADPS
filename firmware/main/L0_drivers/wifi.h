#pragma once
#include "common.h"



/**
 *  @module      : wifi
 *  @description : Initializes wifi as a station
 */

/// Logging struct for wifi information
typedef struct
{
    bool is_connected;
    const char * device_ip;     ///< IP Address
    const char * device_gw;     ///< Gateway
    const char * device_sn;     ///< Subnet Mask
    const char * station_ssid;  ///< Name of station
} wifi_logs_S;

/// Initializes the wifi as an access point
void init_wifi(void);

/// Returns true if wifi is connected to access point, else false
bool wifi_is_connected(void);

/// Returns a pointer to the wifi logging struct
wifi_logs_S * wifi_get_logs(void);
