#include "safezone_log.h"
#include <stdio.h>

void safezone_log_mac(const char *mac_str) {
    printf("[SAFEZONE] Detected MAC: %s\n", mac_str ? mac_str : "UNKNOWN");
    fflush(stdout);
}

void safezone_log_ble_name(const char *ble_name) {
    printf("[SAFEZONE] Generated BLE Name: %s\n", ble_name ? ble_name : "UNKNOWN");
    fflush(stdout);
}

void safezone_log_ble_adv_started(const char *ble_name) {
    printf("[SAFEZONE] BLE Advertising Started: %s\n", ble_name ? ble_name : "UNKNOWN");
    fflush(stdout);
}

void safezone_log_state_alert(const char *zone, const char *reason) {
    printf("[SAFEZONE] State -> HAZARD_ALERT | Zone: %s | Reason: %s | GPIO2=HIGH GPIO4=HIGH (<50ms)\n",
           zone ? zone : "UNKNOWN", reason ? reason : "UNKNOWN");
    fflush(stdout);
}

void safezone_log_state_safe(const char *trigger) {
    printf("[SAFEZONE] State -> SAFE | Trigger: %s | GPIO2=LOW GPIO4=LOW\n",
           trigger ? trigger : "COMMAND_CLEAR");
    fflush(stdout);
}
