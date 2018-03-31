#pragma once



/**
 *  @module      : config
 *  @description : Configuration defines
 */

// Enables testing code
#define TESTING          (1)

// Size of thread pool
#define THREAD_POOL_SIZE (1) // Errors creating more than 9 clients

// Port numbers for client, server, and remote server
#define SERVER_PORT      (5000)
#define CLIENT_PORT      (6000)
#define REMOTE_PORT      (5000)

// Station configuration
#define REMOTE_IP        ("192.168.43.12")
#define DEVICE_IP        ("192.168.43.250")
#define DEVICE_GW        ("192.168.1.1")
#define DEVICE_SN        ("255.255.255.0")

#define MAX_TASKS        (50)