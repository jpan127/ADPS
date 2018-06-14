#include "tasks.h"
// Project libraries
#include "client.h"
#include "packet.h"
#include "wifi.h"
#include "repeater.h"

/**
 *  [task_tx] will create multiple tasks
 *  Each static function here is independent of the calling task and will be used by all
 */



/**
 *  Opens a socket with a timeout
 *  @param client_socket          : The socket handle
 *  @param port                   : The port to open with
 *  @param task_id                : The ID of the calling task
 *  @param delay_between_retry_ms : Delay between retries, can be NULL
 *  @returns                      : Success status
 *  @note                         : Socket creation can return ENOBUFS, in which case, retrying after a delay most likely will succeed
 */
static bool open_socket_with_timeout(int *client_socket, uint32_t port, uint8_t task_id, const uint16_t * delay_between_retry_ms)
{
    const uint16_t two_second_ms = 2000;

    bool client_created = false;
    int8_t timeout_count = 60;

    // If specified, use parameter delay, otherwise, use default one second
    const uint16_t delay_ms = (delay_between_retry_ms) ? (*delay_between_retry_ms) : (two_second_ms);

    // Initialize client
    while (!client_created && timeout_count > 0)
    {
        client_created = tcp_client_create_socket(client_socket, port);

        if (!client_created)
        {
            --timeout_count;
        #if EXTRA_DEBUG_MSGS
            if (timeout_count <= 0)
            {
                ESP_LOGE("init_socket_tx_task", "[%d] FAILED to create client", task_id);
            }
        #endif
            DELAY_MS(delay_ms);
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
    /// @TODO : Perfect place for leaky bucket
    const uint16_t two_second_ms = 2000;
    const uint8_t max_timeout_count = 100;

    bool connected = false;
    int8_t timeout_count = max_timeout_count;

    // Connect to server
    while (!connected && timeout_count > 0)
    {
        connected = tcp_client_connect_to_server(client_socket, (char *)REMOTE_IP, REMOTE_PORT);

        if (!connected)
        {
            --timeout_count;
            if (timeout_count <= 0)
            {
                ESP_LOGE("connect_socket_with_timeout", "[task_tx %d] FAILED to connect client to remote server out of %d retries", task_id, max_timeout_count);
            }
            // Retry in 2 seconds
            DELAY_MS(two_second_ms);
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
static bool init_task_tx(int *client_socket, uint32_t port, uint8_t task_id, const uint16_t * delay_between_retry_ms)
{
    const bool success = (open_socket_with_timeout(client_socket, port, task_id, delay_between_retry_ms) &&
                          connect_socket_with_timeout(client_socket, port, task_id));

    return success;
}

/**
 *  Closes an already open socket, and reopens it
 *  @param client_socket : The socket handle
 *  @param port          : The port to open with
 *  @param task_id       : The ID of the calling task
 *  @returns             : Success status
 */
static bool reopen_socket(int *client_socket, uint32_t port, uint8_t task_id, const uint16_t * delay_between_retry_ms)
{
    tcp_client_close_socket(client_socket);

    return init_task_tx(client_socket, port, task_id, delay_between_retry_ms);
}

void task_tx(task_param_T params)
{
    const uint16_t delay_between_client_reconnect_ms = 100;

    // Each task_tx has its own input parameter with its designated task ID and port number
    const task_tx_params_S task_params = *((task_tx_params_S *)params);

    int client_socket = -1;

    const repeat_S repeat_task =
    {
        .num_retries = 150,
        .delay_ms    = 200,
        .callback    = &wifi_is_connected,
    };

    // Wait until wifi is connected
    if (!repeater_execute(&repeat_task))
    {
        ESP_LOGE("task_tx", "[%d] Wireless is not initialized and client task is terminating...", task_params.task_id);
        vTaskSuspend(NULL);
    }
    // Initialize the client socket first
    else if (!init_task_tx(&client_socket, task_params.port, task_params.task_id, NULL))
    {
        ESP_LOGE("task_tx", "[%d] Client task failed to initialize socket and is terminating...", task_params.task_id);
        vTaskSuspend(NULL);
    }
    else
    {
        ESP_LOGI("task_tx", "[%d] Task starting...", task_params.task_id);
    }

    // Diagnostic packet
    diagnostic_packet_S packet = { 0 };
    uint8_t * const buffer = (uint8_t *)&packet;

    // Size of payload
    uint8_t current_packet_size = 0;

    // Main loop
    while (1)
    {
        // Block until received packet from queue
        xQueueReceive(MessageTxQueue, &packet, MAX_DELAY);

        // Size of packet to send is 1 (type) + 1 (length) + length
        current_packet_size = 2 + packet.length;

        // Send packet, function already prints error message, nothing to handle
        tcp_client_send_packet(client_socket, buffer, current_packet_size);

        // Close and reopen socket, also prints its own error message, nothing to handle
        if (!reopen_socket(&client_socket, task_params.port, task_params.task_id, &delay_between_client_reconnect_ms))
        {
            ESP_LOGE("task_tx", "[%d] FAILED to create client", task_params.task_id);
        }

        // Clear buffer so data doesnt overlap
        // Only need to clear current_packet_size because other bytes are untouched
        memset(&packet, 0, current_packet_size);
    }
}