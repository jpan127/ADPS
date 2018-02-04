#include "log_struct.h"



/// Struct that holds loggin information for all modules
static log_struct_S logs = { NULL };

void log_struct_init(void)
{
    logs.tcp_client_logs = tcp_client_get_logs();
    logs.tcp_server_logs = tcp_server_get_logs();
    logs.packet_logs     = packet_get_logs();
    logs.motor_logs      = motor_get_logs();
    logs.wifi_logs       = wifi_get_logs();
}

log_struct_S * log_struct_get_pointer(void)
{
    return &logs;
}
