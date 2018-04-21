#pragma once

/**
 *  @module      : config
 *  @description : Configuration defines
 */



// Enables testing code
#define TESTING          (1)

// Turns on / off some debug messages that are only helpful sometimes
#define EXTRA_DEBUG_MSGS (0)

// Size of thread pool
#define THREAD_POOL_SIZE (1)

// Port numbers for client, server, and remote server
#define SERVER_PORT      (5000)
#define CLIENT_PORT      (6000)
#define REMOTE_PORT      (5002)

// #define REMOTE_IP        ("172.20.10.3")
// #define DEVICE_IP        ("172.20.43.250")
// #define DEVICE_GW        ("172.20.1.1")
// #define DEVICE_SN        ("255.255.255.0")

// Station configuration
#define REMOTE_IP        ("192.168.43.12")
#define DEVICE_IP        ("192.168.43.250")
#define DEVICE_GW        ("192.168.1.1")
#define DEVICE_SN        ("255.255.255.0")

#define MAX_TASKS        (50)

/// Buffer size for [task_rx]
#define RECV_BUFFER_SIZE (256)
