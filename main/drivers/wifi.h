#pragma once
#include "common.h"



/**
 *  @module      : wifi
 *  @description : Initializes wifi as a station
 */

typedef struct
{
    const char * device_ip;     ///< IP Address
    const char * device_gw;     ///< Gateway
    const char * device_sn;     ///< Subnet Mask
    const char * station_ssid;  ///< Name of station
} wifi_logs_S;

void init_wifi(void);

wifi_logs_S * wifi_get_logs(void);
