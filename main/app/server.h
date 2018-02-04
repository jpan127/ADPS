#pragma once
// Project libraries
#include "common.h"



/**
 *  @module      : server
 *  @description : Socket API as a server
 */

/// State of socket
typedef enum 
{
    socket_unconnected = 0,
    socket_closed,
    socket_created,
    socket_binded,
    socket_listening,
} socket_state_E;

typedef struct
{
    uint32_t server_port;
    int socket;
    socket_state_E state;
    uint32_t queue_size;
    uint32_t packets_received;
} tcp_server_logs_S;

/**
 *
 */
bool tcp_server_init(uint32_t port, uint8_t queue_size);

/**
 *
 */
bool tcp_server_receive(int client_sock, uint8_t *buffer, uint16_t *size);

/**
 *
 */
int tcp_server_get_socket(void);

tcp_server_logs_S * tcp_server_get_logs(void);
