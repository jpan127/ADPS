#include "tasks.h"
// Project libraries
#include "log_struct.h"



/// Enumerate the logging multiplexing
typedef enum
{
    mux_client = 0,
    mux_server,
    mux_packet,
    mux_motor,
    mux_wifi,
    mux_last_invalid,
} logging_mux_E;

void init_task_logger(void)
{
    // Initialize all the pointers of the struct
    log_struct_init();
}

void task_logger(task_param_T params)
{
    // Pointer to the single log struct instance
    const log_struct_S * ptr = log_struct_get_pointer();

    // Split up all the muxes to be able to log everything in the span of 1 second in even intervals
    const uint16_t delay_between_muxes_ms = 1000 / mux_last_invalid;

    // Mux for selecting which logs to send to the server
    logging_mux_E mux = mux_client;

    char buffer[32] = { 0 };

    // Main loop
    while (1)
    {
        /**
         *  Format of these logs: %u1:%s:%u2
         *  %u1 : Category
         *  %s  : Subcategory
         *  %u2 : Value
         */

        switch (mux)
        {
            case mux_client:

                log_data("%u:sockets_created:%u",       log_type_client,    &ptr->tcp_client_logs->sockets_created);
                log_data("%u:sockets_closed:%u",        log_type_client,    &ptr->tcp_client_logs->sockets_closed);
                log_data("%u:server_connections:%u",    log_type_client,    &ptr->tcp_client_logs->server_connections);
                log_data("%u:packets_sent:%u",          log_type_client,    &ptr->tcp_client_logs->packets_sent);
                break;

            case mux_server:

                log_data("%u:server_port:%u",           log_type_server,    &ptr->tcp_server_logs->server_port);
                log_data("%u:state:%u",                 log_type_server,    &ptr->tcp_server_logs->state);
                log_data("%u:queue_size:%u",            log_type_server,    &ptr->tcp_server_logs->queue_size);
                log_data("%u:packets_received:%u",      log_type_server,    &ptr->tcp_server_logs->packets_received);
                break;

            case mux_packet:

                log_data("%u:rx_packets:%u",            log_type_packet,    &ptr->packet_logs->rx_packets);
                log_data("%u:tx_packets:%u",            log_type_packet,    &ptr->packet_logs->tx_packets);
                log_data("%u:packet_errors:%u",         log_type_packet,    &ptr->packet_logs->packet_errors);
                for (uint8_t i=0; i<PACKET_TYPE_LAST_INVALID; i++)
                {
                    snprintf(buffer, sizeof(buffer), "%%u:packet_counts[%u]:%%u", i);
                    log_data(buffer, log_type_packet, &ptr->packet_logs->packet_counts[i]);
                }
                break;

            case mux_motor:

                for (uint8_t i=0; i<motor_max; i++)
                {
                    snprintf(buffer, sizeof(buffer), "%%u:duty_a[%u]:%%f", i);
                    log_data_float(buffer, log_type_motor, &ptr->motor_logs->duty[i].a.percent);

                    snprintf(buffer, sizeof(buffer), "%%u:duty_b[%u]:%%f", i);
                    log_data_float(buffer, log_type_motor, &ptr->motor_logs->duty[i].b.percent);
                }
                break;

            case mux_wifi:

                log_data_string("%u:device_ip:%s",      log_type_wifi,    ptr->wifi_logs->device_ip);
                log_data_string("%u:device_gw:%s",      log_type_wifi,    ptr->wifi_logs->device_gw);
                log_data_string("%u:device_sn:%s",      log_type_wifi,    ptr->wifi_logs->device_sn);
                log_data_string("%u:station_ssid:%s",   log_type_wifi,    ptr->wifi_logs->station_ssid);
                break;

            default:
            
                break;
        }

        // Loop through the muxes
        mux = (mux_wifi == mux) ? (mux_client) : (mux + 1);

        // Delay between next mux
        TickType_t xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil(&xLastWakeTime, delay_between_muxes_ms);
    }
}