#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "mac_reader.h"
#include "ble_name_fmt.h"
#include "ble_adv.h"
#include "uart_cmd_parser.h"
#include "gpio_actuator.h"
#include "safezone_log.h"

#if defined(ESP_PLATFORM)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "esp_timer.h"

#define UART_PORT_NUM      UART_NUM_0
#define UART_BAUD_RATE     115200
#define UART_RX_BUF_SIZE   256
#endif

void app_main(void) {
#if defined(ESP_PLATFORM)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
#endif

    // 1. Initialize GPIO actuator (GPIO 2 LED, GPIO 4 Buzzer/Relay)
    actuator_init();

    // 2. Read dynamic runtime MAC address (FR-002)
    uint8_t mac[6] = {0};
    mac_read_base(mac);
    char mac_str[20] = {0};
    mac_to_string(mac, mac_str, sizeof(mac_str));
    safezone_log_mac(mac_str);

    // 3. Generate dynamic BLE name "FirmGen_XX" (FR-003, FR-004)
    char ble_name[16] = {0};
    build_firmgen_ble_name(mac, ble_name, sizeof(ble_name));
    safezone_log_ble_name(ble_name);

    // 4. Initialize BLE stack and start continuous advertising (FR-001, FR-005)
    ble_adv_init();
    ble_adv_start_continuous(ble_name);
    safezone_log_ble_adv_started(ble_name);

#if defined(ESP_PLATFORM)
    // 5. Configure UART0 at 115200 baud (FR-006, FR-008)
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_driver_install(UART_PORT_NUM, UART_RX_BUF_SIZE * 2, 0, 0, NULL, 0);

    char rx_buffer[128] = {0};
    int rx_idx = 0;

    while (1) {
        uint8_t byte;
        int len = uart_read_bytes(UART_PORT_NUM, &byte, 1, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            if (byte == '\n' || byte == '\r') {
                if (rx_idx > 0) {
                    rx_buffer[rx_idx] = '\0';
                    pipegen_cmd_t cmd;
                    if (parse_pipegen_frame(rx_buffer, &cmd)) {
                        if (cmd.type == CMD_TYPE_ALERT) {
                            actuator_set_hazard_state(SYSTEM_STATE_HAZARD_ALERT);
                            safezone_log_state_alert(cmd.zone, cmd.reason);
                        } else if (cmd.type == CMD_TYPE_CLEAR) {
                            actuator_set_hazard_state(SYSTEM_STATE_SAFE);
                            safezone_log_state_safe("COMMAND_CLEAR");
                        }
                    }
                    rx_idx = 0;
                }
            } else if (rx_idx < (sizeof(rx_buffer) - 1)) {
                rx_buffer[rx_idx++] = (char)byte;
            }
        }

        // Hardware failsafe watchdog check (10s window)
        uint32_t now_ms = (uint32_t)(esp_timer_get_time() / 1000);
        if (actuator_failsafe_tick(now_ms)) {
            safezone_log_state_safe("FAILSAFE_10S_TIMEOUT");
        }
    }
#else
    printf("[SAFEZONE] Host execution mode running.\n");
#endif
}
