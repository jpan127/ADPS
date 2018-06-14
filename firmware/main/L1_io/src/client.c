#include "client.h"
// lwIP libraries
#include "lwip/inet.h"
#include "lwip/sockets.h"



/// Logging struct for TCP client
static tcp_client_logs_S logs = { 0 };

bool tcp_client_create_socket(int * tcp_socket, const uint32_t port)
{
    // Counter for getting error ENOBUFS
    static uint32_t no_buffer_counter = 0;
    // Flag error if received ENOBUFS 10 times in a row
    const uint8_t no_buffer_error_limit = 10;

    bool success = false;

    // Close just in case the socket is already open
    tcp_client_close_socket(tcp_socket);

    // Create socket
    *tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (*tcp_socket < 0)
    {
        // Increment counter
        no_buffer_counter = (ENOBUFS == errno) ? (no_buffer_counter + 1) : (0);
        // Send an error after it reaches the limit, and continue to send the error for each subsequent failure
        if (no_buffer_counter >= no_buffer_error_limit)
        {
            ESP_LOGE("tcp_client_create_socket", "Error creating socket: %s | Port: %u", strerror(errno), port);
        }
    }
    else
    {
        // Set socket option to re-usable
        int option = 0x1;
        setsockopt(*tcp_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        // ESP_LOGI("tcp_create_socket", "Socket successfully created.");
        logs.sockets_created++;
        success = true;
        no_buffer_counter = 0;
    }

    return success;
}

void tcp_client_close_socket(int * tcp_socket)
{
    if (*tcp_socket > 0)
    {
        // Close socket
        shutdown(*tcp_socket, SHUT_RDWR);
        close(*tcp_socket);

        // Reset value
        *tcp_socket = -1;

        logs.sockets_closed++;
    }
}

bool tcp_client_connect_to_server(int * tcp_socket, const char * server_ip, const uint16_t server_port)
{
    bool success = false;

    // Create socket address struct for the server's address
    // inet_pton(AF_INET, server_ip, &server_address.sin_addr.s_addr);
    const struct sockaddr_in server_address =
    {
        .sin_len         = 0,
        .sin_family      = AF_INET,
        .sin_addr.s_addr = inet_addr(server_ip),
        .sin_port        = htons(server_port),
        .sin_zero        = { 0 },
    };

    // Connect to the server
    if (connect(*tcp_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in)) < 0)
    {
        ESP_LOGE("tcp_connect_to_server", "Error connecting to server at %s port %i | Error: %s", server_ip, server_port, strerror(errno));
        tcp_client_close_socket(tcp_socket);
    }
    else
    {
        // LOG_STATUS("Successfully connected to server at %s port %i", server_ip,  server_port);
        logs.server_connections++;
        success = true;
    }

    return success;
}

bool tcp_client_send_packet(const int tcp_socket, const uint8_t * packet, const uint8_t size)
{
    const int bytes = send(tcp_socket, packet, size, 0);

    if (bytes < 0)          { ESP_LOGE("tcp_send_packet", "Error sending packet. Error: %s", strerror(errno)); }
    else if (bytes < size)  { ESP_LOGE("tcp_send_packet", "Only sent %d / %d bytes.", bytes, size);            }
    else                    { logs.packets_sent++;                                                             }

    return (bytes >= size);
}

tcp_client_logs_S * tcp_client_get_logs(void)
{
    return &logs;
}
