// #include "tasks.hpp"

// enum gatts_profile_id_E
// {
//     PROFILE_A = 0,
//     PROFILE_B = 1,
// };

// typedef struct
// {
//     esp_gatts_cb_t gatts_cb;            // Event handler callback
//     uint16_t gatts_if;                  // Gatt interface type
//     uint16_t app_id;                    // Application ID
//     uint16_t conn_id;
//     uint16_t service_handle;
//     esp_gatt_srvc_id_t service_id;
//     uint16_t char_handle;
//     esp_bt_uuid_t char_uuid;
//     esp_gatt_perm_t perm;
//     esp_gatt_char_prop_t property;
//     uint16_t descr_handle;
//     esp_bt_uuid_t descr_uuid;
// } gatts_profile_info_S;

// static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
// {

// }

// static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
// {

// }


// void BluetoothTask(void *p)
// {
//     // Initialize server
//     esp_bt_controller_config_t config = {
//         .controller_task_stack_size = 8196,
//         .controller_task_prio       = 1,
//         .hci_uart_no                = 0,
//         .hci_uart_baudrate          = 115200,
//     };

//     ESP_ERROR_CHECK(esp_bt_controller_init(&config));
//     ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

//     // Initialize service
//     ESP_ERROR_CHECK(esp_bluedroid_init());
//     ESP_ERROR_CHECK(esp_bluedroid_enable());
//     ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_event_handler));
//     ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
//     ESP_ERROR_CHECK(esp_ble_gatts_app_register(PROFILE_A));
//     ESP_ERROR_CHECK(esp_ble_gatts_app_register(PROFILE_B));
//     ESP_ERROR_CHECK(esp_ble_gatt_set_local_mtu(500));

//     // Initialize characteristics

//     // Listen
// }