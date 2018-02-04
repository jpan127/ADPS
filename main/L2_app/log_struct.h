#pragma once
// Project libraries
#include "common.h"
#include "client.h"
#include "server.h"
#include "motor.h"
#include "packet.h"
#include "wifi.h"



/**
 *  @module      : log_struct
 *  @description : A structure of all loggable information
 */

/// Integrated logging struct with pointers to all other logging structs
typedef struct
{
    tcp_client_logs_S * tcp_client_logs;
    tcp_server_logs_S * tcp_server_logs;
    packet_logs_S * packet_logs;
    motor_logs_S * motor_logs;
    wifi_logs_S * wifi_logs;
} log_struct_S;

/// Initializes the struct's pointers
void log_struct_init(void);

/// Returns a pointer to the single instance of the struct
log_struct_S * log_struct_get_pointer(void);
