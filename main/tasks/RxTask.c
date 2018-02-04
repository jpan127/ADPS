#include "tasks.h"
// lwIP libraries
#include "lwip/inet.h"         // inet functions, socket structs
#include "lwip/sockets.h"      // Sockets
// Project libraries
#include "server.h"
#include "packet.h"
#include "motor.h"


#define RECV_BUFFER_SIZE (256)

// Extern
QueueHandle_t ServerQueue;

void Init_RxTask(void)
{
    ServerQueue = xQueueCreate(5, sizeof(command_packet_S));
}

static int accept_blocking(uint8_t task_id, int server_socket)
{
    // Structure to store client details
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);

    // Accept connection and save client socket handle
    int client_socket = accept( server_socket, 
                                (struct sockaddr *)&client_address, 
                                &client_address_size);

    // Error accepting
    if (client_socket < 0) 
    {
        ESP_LOGE("accept_blocking", "[%d] Error accepting from client | Server Socket: %i | Error: %s", 
                                                                                        task_id, 
                                                                                        server_socket, 
                                                                                        strerror(errno));
        return -1;
    }
    // Successfully accepted
    else 
    {
        // ESP_LOGI("accept_blocking", "[%d] Accepted client connection | Server Socket: %i | Client Socket: %i", 
        //                                                                                 task_id, 
        //                                                                                 server_socket, 
        //                                                                                 client_socket);
        return client_socket;
    }
}

void RxTask(void *p)
{
    // This task takes an input parameter which designates its task ID
    const uint8_t task_id = *((uint8_t *)p);
    ESP_LOGI("RxTask", "Task #%d successfully created", task_id);

    // Buffer needed to receive packets
    uint8_t buffer[RECV_BUFFER_SIZE] = { 0 };
    uint16_t size = 0;

    // Parser status
    parser_status_E status = PARSER_IDLE;
    // Command packet
    command_packet_S packet = { 0 };

    // Wait for server to be created before starting
    xEventGroupWaitBits(StatusEventGroup,   // Event group handle
                        BIT_SERVER_READY,   // Bits to wait for
                        false,              // Clear on exit
                        pdTRUE,             // Wait for all bits
                        TICK_MS(ONE_MIN));  // Ticks to wait

    // Get server socket number
    int server_socket = tcp_server_get_socket();

    // Main loop
    while (1)
    {
        // Accept connections, blocking
        int client_socket = accept_blocking(task_id, server_socket);

        // If connection and no error
        if (client_socket)
        {
            // Receive into buffer
            tcp_server_receive(client_socket, buffer, &size);

            // Parse each byte from buffer
            for (uint16_t i=0; i<size; i++)
            {
                status = command_packet_parser(buffer[i], &packet);
                if (PARSER_COMPLETE == status)
                {
                    // Send to uart_tx_task
                    xQueueSend(ServerQueue, &packet, MAX_DELAY);
                }
            }
    
            // Close client socket
            shutdown(client_socket, SHUT_RDWR);
            close(client_socket);
        }
    }
}

static void service_read_command(command_packet_S *packet)
{
    switch (packet->opcode)
    {
        case COMMAND_GET_FORWARD:  motor_move(MOTOR_WHEELS, MOTOR_DIRECTION_FORWARD,  packet->command.bytes[0]); break;
        case COMMAND_GET_BACKWARD: motor_move(MOTOR_WHEELS, MOTOR_DIRECTION_BACKWARD, packet->command.bytes[0]); break;
        case COMMAND_GET_LEFT:     motor_move(MOTOR_WHEELS, MOTOR_DIRECTION_LEFT,     packet->command.bytes[0]); break;
        case COMMAND_GET_RIGHT:    motor_move(MOTOR_WHEELS, MOTOR_DIRECTION_RIGHT,    packet->command.bytes[0]); break;
    }
}
static void service_write_command(command_packet_S *packet)
{
    switch (packet->opcode)
    {
        case COMMAND_SET_FORWARD:  motor_move(MOTOR_WHEELS, MOTOR_DIRECTION_FORWARD,  packet->command.bytes[0]); break;
        case COMMAND_SET_BACKWARD: motor_move(MOTOR_WHEELS, MOTOR_DIRECTION_BACKWARD, packet->command.bytes[0]); break;
        case COMMAND_SET_LEFT:     motor_move(MOTOR_WHEELS, MOTOR_DIRECTION_LEFT,     packet->command.bytes[0]); break;
        case COMMAND_SET_RIGHT:    motor_move(MOTOR_WHEELS, MOTOR_DIRECTION_RIGHT,    packet->command.bytes[0]); break;
    }
}

void CommandTask(void *p)
{
    // Command packet
    command_packet_S packet = { 0 };

    // Main Loop
    while (1)
    {
        // Receive from RxTask
        xQueueReceive(ServerQueue, &packet, MAX_DELAY);

        // Service command
        if (PACKET_TYPE_COMMAND_READ == packet.type)
        {
            service_read_command(&packet);
        }
        else
        {
            service_write_command(&packet);
        }
    }
}