#include "tasks.h"
// Project libraries
#include "client.h"
#include "packet.h"



/**
 *  [task_tx] will create multiple tasks
 *  Each static function here is independent of the calling task and will be used by all
 */

/**
 *  Opens a socket with a timeout
 *  @param client_socket : The socket handle
 *  @param port          : The port to open with
 *  @param task_id       : The ID of the calling task
 *  @returns             : Success status
 */
static bool open_socket_with_timeout(int *client_socket, uint32_t port, uint8_t task_id)
{
    bool client_created = false;
    int timeout_count = 10;
    const uint16_t one_second_ms = 1000;

    // Initialize client
    while (!client_created && timeout_count > 0)
    {
        client_created = tcp_client_create_socket(client_socket, port);

        if (!client_created)
        {
            if (--timeout_count <= 0)
            {
                ESP_LOGE("init_socket_tx_task", "[%d] FAILED to create client", task_id);
            }
            // Retry in 1 second
            DELAY_MS(one_second_ms);
        }
    }

    return client_created;
}

/**
 *  Connects a socket to a server with a timeout
 *  @param client_socket : The socket handle
 *  @param port          : The port to open with
 *  @param task_id       : The ID of the calling task
 *  @returns             : Success status
 */
static bool connect_socket_with_timeout(int *client_socket, uint32_t port, uint8_t task_id)
{
    bool connected = false;
    int timeout_count = 10;
    const uint16_t one_second_ms = 1000;

    // Connect to server
    while (!connected && timeout_count > 0)
    {
        connected = tcp_client_connect_to_server(client_socket, (char *)REMOTE_IP, REMOTE_PORT);

        if (!connected)
        {
            if (--timeout_count <= 0)
            {
                ESP_LOGE("connect_socket_with_timeout", "[%d] FAILED to connect client to remote server", task_id);
            }
            // Retry in 1 second
            DELAY_MS(one_second_ms);
        }
    }

    return connected;
}

/**
 *  Initializes a [task_tx] by creating a TCP client socket
 *  @param client_socket : The socket handle
 *  @param port          : The port to open with
 *  @param task_id       : The ID of the calling task
 *  @returns             : Success status
 *  @note                : Needs to be initialized after scheduler starts because it depends on the tasks input parameters
 */
static bool init_task_tx(int *client_socket, uint32_t port, uint8_t task_id)
{
    bool success = false;

    success = open_socket_with_timeout(client_socket, port, task_id);

    if (success) success = connect_socket_with_timeout(client_socket, port, task_id);

    return success;
}

/**
 *  Closes an already open socket, and reopens it
 *  @param client_socket : The socket handle
 *  @param port          : The port to open with
 *  @param task_id       : The ID of the calling task
 *  @returns             : Success status
 */
static bool reopen_socket(int *client_socket, uint32_t port, uint8_t task_id)
{
    tcp_client_close_socket(client_socket);

    return init_task_tx(client_socket, port, task_id);
}

void task_tx(task_param_T params)
{
    // Each task_tx has its own input parameter with its designated task ID and port number
    const task_tx_params_S task_params = *((task_tx_params_S *)params);

    // Initialize the client socket first
    int client_socket = -1;
    if (!init_task_tx(&client_socket, task_params.port, task_params.task_id))
    {
        ESP_LOGE("task_tx", "[%d] Client task failed to initialize socket and is terminating...", task_params.task_id);
        vTaskSuspend(NULL);
    }
    else
    {
        ESP_LOGI("task_tx", "[%d] Task initialized", task_params.task_id);
    }

    // Diagnostic packet
    diagnostic_packet_S packet = { 0 };
    uint8_t * const buffer = (uint8_t *)&packet;

    // Size of payload
    uint8_t current_packet_size = 0;

    // Main loop
    while (1)
    {
        // // Block until received packet from queue
        // xQueueReceive(MessageTxQueue, &packet, MAX_DELAY);

        packet.type = 5;
        packet.length = 5;
        packet.payload[0] = 'h';
        packet.payload[1] = 'e';
        packet.payload[2] = 'l';
        packet.payload[3] = 'l';
        packet.payload[4] = 'o';
        current_packet_size = 5;

        // Size of packet to send is 1 (type) + 1 (length) + length
        current_packet_size = 2 + packet.length;

        // Send packet, function already prints error message, nothing to handle
        tcp_client_send_packet(&client_socket, buffer, current_packet_size);

        // Close and reopen socket, also prints its own error message, nothing to handle
        reopen_socket(&client_socket, task_params.port, task_params.task_id);

        // Clear buffer so data doesnt overlap
        // Only need to clear current_packet_size because other bytes are untouched
        // memset(&packet, 0, current_packet_size);

        DELAY_MS(1000);
    }
}