#pragma once
// Project libraries
#include "common.h"



/**
 *  @module      : client
 *  @description : Socket API as a client
 */

/// Client logging struct
typedef struct
{
    uint32_t sockets_created;
    uint32_t sockets_closed;
    uint32_t server_connections;
    uint32_t packets_sent;
} tcp_client_logs_S;

/**
 *  Creates a client socket
 *  @param tcp_socket : Socket handle
 *  @param port       : The port number to bind the client socket to
 *  @returns          : Success status
 */
bool tcp_client_create_socket(int * tcp_socket, const uint32_t port);

/**
 *  Closes an open TCP socket
 *  @param tcp_socket : Socket handle
 */
void tcp_client_close_socket(int * tcp_socket);

/**
 *  Connects a client socket to a server
 *  @param tcp_socket  : Socket handle
 *  @param server_ip   : IP address of the server to connect to
 *  @param server_port : Port number of server to connect to
 *  @returns           : Success status
 */
bool tcp_client_connect_to_server(int * tcp_socket, const char * server_ip, const uint16_t server_port);

/**
 *  Sends a packet after it has already connected to a server
 *  @param tcp_socket : Socket handle
 *  @param packet     : Packet to send
 *  @param size       : Size of packet
 *  @returns          : Success status
 */
bool tcp_client_send_packet(const int tcp_socket, const uint8_t * packet, const uint8_t size);

/// Returns a pointer to the TCP client logging struct
tcp_client_logs_S * tcp_client_get_logs(void);
