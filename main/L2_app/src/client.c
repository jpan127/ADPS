#include "client.h"
// lwIP libraries
#include "lwip/inet.h"         // inet functions, socket structs
#include "lwip/sockets.h"      // Sockets



static tcp_client_logs_S logs =
{
    .sockets_created    = 0,
    .sockets_closed     = 0,
    .server_connections = 0,
    .packets_sent       = 0,
};

bool tcp_client_create_socket(int *tcp_socket, uint32_t port)
{
    // Create socket
    *tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (*tcp_socket < 0) 
    {
        ESP_LOGE("tcp_create_socket", "Error creating socket: %s", strerror(errno));
        return false;
    }
    else 
    {
        // Set socket option to re-usable
        int option = 1;
        setsockopt(*tcp_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        // ESP_LOGI("tcp_create_socket", "Socket successfully created.");
        logs.sockets_created++;
        return true;
    }
}

void tcp_client_close_socket(int *tcp_socket)
{
    if (*tcp_socket)
    {
        // Close socket
        shutdown(*tcp_socket, SHUT_RDWR);
        close(*tcp_socket);
        logs.sockets_closed++;
        // Reset value
        *tcp_socket = -1;
    }
}

bool tcp_client_connect_to_server(int *tcp_socket, char *server_ip, uint16_t server_port)
{
    // Create socket address struct for the server's address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    // inet_pton(AF_INET, server_ip, &server_address.sin_addr.s_addr);
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port = htons(server_port);

    // Connect to the server
    if (connect(*tcp_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in)) < 0)
    {
        ESP_LOGE("tcp_connect_to_server", "Error connecting to server at %s port %i. Error: %s",
                                                                                    server_ip, 
                                                                                    server_port, 
                                                                                    strerror(errno));
        tcp_client_close_socket(tcp_socket);
        return false;
    }
    else 
    {
        // ESP_LOGI("tcp_connect_to_server", "Successfully connected to server at %s port %i.",
        //                                                                             server_ip, 
        //                                                                             server_port);
        logs.server_connections++;
        return true;
    }
}

bool tcp_client_send_packet(int *tcp_socket, uint8_t *packet, uint8_t size)
{
    int bytes = send(*tcp_socket, packet, size, 0);
    if (bytes < 0)
    {
        ESP_LOGE("tcp_send_packet", "Error sending packet. Error: %s", strerror(errno));
        return false;
    }
    else if (bytes < size)
    {
        ESP_LOGE("tcp_send_packet", "Only sent %d / %d bytes.", bytes, size);
        return false;
    }
    else 
    {
        // ESP_LOGI("tcp_send_packet", "Packet sent: %s", packet);
        logs.packets_sent++;
        return true;
    }
}

tcp_client_logs_S * tcp_client_get_logs(void)
{
    return &logs;
}