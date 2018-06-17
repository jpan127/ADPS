#include "tasks.h"
// Standard libraries
#include <math.h>
// Project libraries
#include "common.h"
#include "client.h"
#include "server.h"
#include "motor.h"
#include "packet.h"
#include "wifi.h"
#include "infrared.h"



/// Enumerate the logging multiplexing
typedef enum
{
    mux_first_invalid = 0,
    mux_client,
    mux_server,
    mux_packet,
    mux_motor,
    mux_wifi,
    mux_task_watermarks,
    mux_infrared_distances,
    mux_last_invalid,
} logging_mux_E;

/// Single instance of log pointers
static struct
{
    tcp_client_logs_S * tcp_client_logs;
    tcp_server_logs_S * tcp_server_logs;
    packet_logs_S * packet_logs;
    motor_logs_S * motor_logs;
    wifi_logs_S * wifi_logs;
    infrared_logs_S * infrared_logs;
} logs;

void init_task_logger(void)
{
    // Initialize all the pointers of the struct
    logs.tcp_client_logs = tcp_client_get_logs();
    logs.tcp_server_logs = tcp_server_get_logs();
    logs.packet_logs     = packet_get_logs();
    logs.motor_logs      = motor_get_logs();
    logs.wifi_logs       = wifi_get_logs();
    logs.infrared_logs   = infrared_get_logs();
}

void task_logger(task_param_T params)
{
    // Wait a second before starting to log, waits for task handles to be initialized properly
    DELAY_MS(1000);

    // All the muxes are time multplexed across this period
    const uint16_t delay_period_ms = 1000;

    // Split up all the muxes to be able to log everything in the span of 1 second in even intervals
    const uint16_t delay_between_muxes_ms = delay_period_ms / mux_last_invalid;

    // Mux for selecting which logs to send to the server
    logging_mux_E mux = mux_client;

    char buffer[64] = { 0 };

    // Get task handles to check watermarks
    rtos_task_context_block_S * tcbs = NULL;
    size_t num_tasks = 0;
    tasks_get_tcbs(&tcbs, &num_tasks);

    ESP_LOGI("task_logger", "Task starting...");

    // Main loop
    while (1)
    {
        /**
         *  Format of these logs: %u1:%s:%u2
         *      - %u1 : Category
         *      - %s  : Subcategory
         *      - %u2 : Value
         */

        switch (mux)
        {
            case mux_client:
            {
                log_data("%u:sockets_created:%u",       packet_type_log_client,    &logs.tcp_client_logs->sockets_created);
                log_data("%u:sockets_closed:%u",        packet_type_log_client,    &logs.tcp_client_logs->sockets_closed);
                log_data("%u:server_connections:%u",    packet_type_log_client,    &logs.tcp_client_logs->server_connections);
                log_data("%u:packets_sent:%u",          packet_type_log_client,    &logs.tcp_client_logs->packets_sent);
                break;
            }
            case mux_server:
            {
                log_data("%u:server_port:%u",           packet_type_log_server,    &logs.tcp_server_logs->server_port);
                log_data("%u:state:%u",                 packet_type_log_server,    &logs.tcp_server_logs->state);
                log_data("%u:queue_size:%u",            packet_type_log_server,    &logs.tcp_server_logs->queue_size);
                log_data("%u:packets_received:%u",      packet_type_log_server,    &logs.tcp_server_logs->packets_received);
                break;
            }
            case mux_packet:
            {
                log_data("%u:rx_packets:%u",            packet_type_log_packet,    &logs.packet_logs->rx_packets);
                log_data("%u:tx_packets:%u",            packet_type_log_packet,    &logs.packet_logs->tx_packets);
                log_data("%u:packets_dropped:%u",       packet_type_log_packet,    &logs.packet_logs->packets_dropped);
                break;
            }
            case mux_motor:
            {
                for (motor_E motor = (motor_E)0; motor < motor_max; motor++)
                {
                    snprintf(buffer, sizeof(buffer), "%%u:duty_a%u:%%f", motor);
                    log_data_float(buffer, packet_type_log_motor, &logs.motor_logs->duty[motor].a);

                    snprintf(buffer, sizeof(buffer), "%%u:duty_b%u:%%f", motor);
                    log_data_float(buffer, packet_type_log_motor, &logs.motor_logs->duty[motor].b);
                }
                break;
            }
            case mux_wifi:
            {
                log_data_string("%u:device_ip:%s",      packet_type_log_wifi,    logs.wifi_logs->device_ip);
                log_data_string("%u:device_gw:%s",      packet_type_log_wifi,    logs.wifi_logs->device_gw);
                log_data_string("%u:device_sn:%s",      packet_type_log_wifi,    logs.wifi_logs->device_sn);
                log_data_string("%u:station_ssid:%s",   packet_type_log_wifi,    logs.wifi_logs->station_ssid);
                break;
            }
            case mux_task_watermarks:
            {
                for (size_t i=0; i<num_tasks; i++)
                {
                    // Lookup task name from handle
                    const char * const task_name = pcTaskGetTaskName(tcbs[i].handle);

                    // Get watermark
                    const uint32_t watermark = uxTaskGetStackHighWaterMark(tcbs[i].handle);

                    // Calculate how much of the allocated stack size was ever used
                    const float stack_utilization = (float)(tcbs[i].stack_size - watermark) / (float)tcbs[i].stack_size;

                    snprintf(buffer, sizeof(buffer), "%%u:%s:%%u", task_name);

                    log_data_float(buffer, packet_type_log_wmark, &stack_utilization);
                }
                break;
            }
            case mux_infrared_distances:
            {
                static const char * const infrared_str = "%%u:infrared%u:%%u";
                for (infrared_E ir = (infrared_E)0; ir < infrared_max; ir++)
                {
                    snprintf(buffer, sizeof(buffer), infrared_str, ir);
                    log_data(buffer, packet_type_log_infrared, (const uint32_t *)&logs.infrared_logs[ir].operational);
                    log_data(buffer, packet_type_log_infrared, (const uint32_t *)&logs.infrared_logs[ir].raw_values);
                    log_data(buffer, packet_type_log_infrared, (const uint32_t *)&logs.infrared_logs[ir].distances);
                }
                break;
            }
            default:
            {
                LOG_ERROR("Undefined logger mux specified %d", mux);
                break;
            }
        }

        // Iterate through the muxes
        mux = (mux_last_invalid >= mux+1) ? (mux_first_invalid + 1) : (mux + 1);

        // Delay between next mux
        DELAY_MS(delay_between_muxes_ms);
    }
}
