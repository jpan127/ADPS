#include "tasks.h"
#include "server.h"
#include "rom/ets_sys.h"    // ets_delay_us()
#include "packet.h"


/// Extern
QueueHandle_t MessageRxQueue = NULL;
QueueHandle_t MessageTxQueue = NULL;

/// Size of message queues
static const uint8_t message_queue_size = 5;

void init_server_socket(void)
{
    const uint8_t queue_size  = 5;
    const uint32_t one_second = 1000 * 1000;
    bool server_created       = false;
    int timeout_count         = 10;

    while (!server_created)
    {
        server_created = tcp_server_init(SERVER_PORT, queue_size);
        if (server_created)
        {
            xEventGroupSetBits( StatusEventGroup, BIT_SERVER_READY );
            ESP_LOGI("init_server_socket", "SUCCESSFULLY initialized server");
            break;
        }
        else
        {
            if (--timeout_count <= 0)
            {
                ESP_LOGE("init_server_socket", "FAILED to initialize server");
                break;
            }
            // Retry in 1 second
            ets_delay_us(one_second);
        }
    }
}

void init_create_all_queues(void)
{
    MessageRxQueue = xQueueCreate(message_queue_size, sizeof(command_packet_S));
    MessageTxQueue = xQueueCreate(message_queue_size, sizeof(diagnostic_packet_S));
}