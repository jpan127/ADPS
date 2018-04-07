#include "tasks.h"
// lwIP libraries
#include "lwip/inet.h"
#include "lwip/sockets.h"
// Project libraries
#include "client.h"
#include "server.h"
#include "packet.h"
#include "motor.h"
#include "cmd_handler.h"



/**
 *  Accepts an incoming client connection
 *  @param task_id       : ID of the calling task
 *  @param server_socket : Socket handle of the server
 *  @returns             : Socket handle of the client
 *  @note                : The accept() call is blocking
 */
static int accept_blocking(uint8_t task_id, const int server_socket)
{
    // Structure to store client details
    struct sockaddr_in client_address = { 0 };
    socklen_t client_address_size = sizeof(client_address);

    // Accept connection and save client socket handle
    const int client_socket = accept( server_socket, 
                                      (struct sockaddr *)&client_address, 
                                      &client_address_size);

    // Error accepting
    if (client_socket < 0) 
    {
        ESP_LOGE("accept_blocking", "[%d] Error accepting from client | Server Socket: %i | Error: %s", task_id, server_socket, strerror(errno));
    }
    else
    {
        ESP_LOGI("accept_blocking", "[%d] Accepted from client | Server Socket: %i", task_id, server_socket);
    }

    return client_socket;
}

/// @TODO : Change all logging to go through [packet]
void task_rx(task_param_T params)
{
    // This task takes an input parameter which designates its task ID
    const uint8_t task_id = *((uint8_t *)params);

    // Wait for server to be created before starting
    if (!xEventGroupWaitBits(StatusEventGroup,   ///< Event group handle
                             BIT_SERVER_READY,   ///< Bits to wait for
                             false,              ///< Clear on exit
                             pdTRUE,             ///< Wait for all bits
                             NO_DELAY))          ///< No need to wait since server is created before scheduler even starts,
                                                 ///< if it didn't create successfully then, it never will be
    {
        ESP_LOGE("task_rx", "Suspending task #%d because server was not created.", task_id);
        vTaskSuspend(NULL);
    }
    else
    {
        ESP_LOGI("task_rx", "[%d] Task starting...", task_id);
    }

    // Buffer needed to receive packets
    uint8_t buffer[RECV_BUFFER_SIZE] = { 0 };

    // Size of packet
    uint16_t size = 0;

    // Parser status
    parser_status_E status = parser_status_idle;

    // Command packet
    command_packet_S packet = { 0 };

    // Get server socket number
    const int server_socket = tcp_server_get_socket();

    // Main loop
    while (1)
    {
        // Accept a client connection
        int client_socket = accept_blocking(task_id, server_socket);

        // If connection and no error
        if (client_socket > 0)
        {
            // Receive into buffer
            tcp_server_receive(client_socket, buffer, &size);

            status = command_packet_parser(buffer, &packet);

            if (parser_status_complete == status)
            {
                ESP_LOGI("task_rx", "Servicing command : %u %u", packet.opcode, packet.command[0]);
                cmd_handler_service_command(&packet);
            }
    
            tcp_client_close_socket(&client_socket);
        }
    }
}
