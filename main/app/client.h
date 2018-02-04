#pragma once
// Project libraries
#include "common.h"


/**
 *  @module      : client
 *  @description : Socket API as a client
 */

typedef struct
{
    uint32_t sockets_created;
    uint32_t sockets_closed;
    uint32_t server_connections;
    uint32_t packets_sent;
} tcp_client_logs_S;

bool tcp_client_create_socket(int *tcp_socket, uint32_t port);

void tcp_client_close_socket(int *tcp_socket);

bool tcp_client_connect_to_server(int *tcp_socket, char *server_ip, uint16_t server_port);

bool tcp_client_send_packet(int *tcp_socket, uint8_t *packet, uint8_t size);

tcp_client_logs_S * tcp_client_get_logs(void);
