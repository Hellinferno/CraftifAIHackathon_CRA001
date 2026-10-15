#include "mac_reader.h"
#include <stdio.h>
#include <string.h>

#if defined(ESP_PLATFORM)
#include "esp_mac.h"
#include "esp_system.h"
#endif

bool mac_read_base(uint8_t mac[6]) {
    if (!mac) return false;

#if defined(ESP_PLATFORM)
    esp_err_t ret = esp_read_mac(mac, ESP_MAC_BT);
    if (ret != ESP_OK) {
        ret = esp_read_mac(mac, ESP_MAC_WIFI_STA);
    }
    return (ret == ESP_OK);
#else
    // Host mock / unit testing fallback
    static uint8_t mock_mac[6] = {0xA4, 0xCF, 0x12, 0x34, 0x56, 0x9B};
    memcpy(mac, mock_mac, 6);
    return true;
#endif
}

bool mac_to_string(const uint8_t mac[6], char *out_str, size_t max_len) {
    if (!mac || !out_str || max_len < 18) return false;
    snprintf(out_str, max_len, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return true;
}
