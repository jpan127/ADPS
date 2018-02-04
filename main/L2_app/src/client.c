#include "client.h"
// lwIP libraries
#include "lwip/inet.h"
#include "lwip/sockets.h"



/// Logging struct for TCP client
static tcp_client_logs_S logs =
{
    .sockets_created    = 0,
    .sockets_closed     = 0,
    .server_connections = 0,
    .packets_sent       = 0,
};

bool tcp_client_create_socket(int *tcp_socket, uint32_t port)
{
    bool success = false;

    // Create socket
    *tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (*tcp_socket < 0) 
    {
        ESP_LOGE("tcp_create_socket", "Error creating socket: %s", strerror(errno));
    }
    else 
    {
        // Set socket option to re-usable
        int option = 1;
        setsockopt(*tcp_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        // ESP_LOGI("tcp_create_socket", "Socket successfully created.");
        logs.sockets_created++;
        success = true;
    }

    return success;
}

void tcp_client_close_socket(int *tcp_socket)
{
    if (*tcp_socket > 0)
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
    bool success = false;

    // Create socket address struct for the server's address
    // inet_pton(AF_INET, server_ip, &server_address.sin_addr.s_addr);
    const struct sockaddr_in server_address =
    {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = inet_addr(server_ip),
        .sin_port        = htons(server_port),
    };

    // Connect to the server
    if (connect(*tcp_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in)) < 0)
    {
        ESP_LOGE("tcp_connect_to_server", "Error connecting to server at %s port %i. Error: %s", server_ip, server_port, strerror(errno));
        tcp_client_close_socket(tcp_socket);
    }
    else 
    {
        // ESP_LOGI("tcp_connect_to_server", "Successfully connected to server at %s port %i.", server_ip,  server_port);
        logs.server_connections++;
        success = true;
    }

    return success;
}

bool tcp_client_send_packet(int *tcp_socket, uint8_t *packet, uint8_t size)
{
    const int bytes = send(*tcp_socket, packet, size, 0);

    const bool success = (bytes >= size);

    if (bytes < 0)          { ESP_LOGE("tcp_send_packet", "Error sending packet. Error: %s", strerror(errno)); }
    else if (bytes < size)  { ESP_LOGE("tcp_send_packet", "Only sent %d / %d bytes.", bytes, size);            }
    else                    { logs.packets_sent++;                                                             }

    return success;
}

tcp_client_logs_S * tcp_client_get_logs(void)
{
    return &logs;
}