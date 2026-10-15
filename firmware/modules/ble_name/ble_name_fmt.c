#include "ble_name_fmt.h"
#include <stdio.h>
#include <string.h>

bool extract_mac_suffix(const uint8_t mac[6], char *suffix_out, size_t max_len) {
    if (!mac || !suffix_out || max_len < 3) return false;
    // Extract last byte (mac[5]) as 2 uppercase hex digits
    snprintf(suffix_out, max_len, "%02X", mac[5]);
    return true;
}

bool build_firmgen_ble_name(const uint8_t mac[6], char *name_out, size_t max_len) {
    if (!mac || !name_out || max_len < 11) return false;
    char suffix[4] = {0};
    if (!extract_mac_suffix(mac, suffix, sizeof(suffix))) {
        return false;
    }
    snprintf(name_out, max_len, "FirmGen_%s", suffix);
    return true;
}
