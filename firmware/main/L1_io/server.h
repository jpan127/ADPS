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

/// TCP server logging struct
typedef struct
{
    uint32_t server_port;
    int socket;
    socket_state_E state;
    uint32_t queue_size;
    uint32_t packets_received;
} tcp_server_logs_S;

/**
 *  Initializes a single socket as a TCP server
 *  @param port       : The port number of the server
 *  @param queue_size : The queue size to listen for
 */
bool tcp_server_init(uint32_t port, uint8_t queue_size);

/**
 *  Receive bytes from an accepted client connection into a buffer
 *  @param client_sock : The client sock connected
 *  @param buffer      : Buffer to receive into
 *  @param size        : Size of buffer
 *  @returns           : False if any error encountered
 */
bool tcp_server_receive(int client_sock, uint8_t *buffer, int *size);

/// Returns the socket handle
int tcp_server_get_socket(void);

/// Returns a pointer to the TCP server logging struct
tcp_server_logs_S * tcp_server_get_logs(void);
