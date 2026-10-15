#ifndef SAFEZONE_LOG_H
#define SAFEZONE_LOG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logs detected MAC address at boot.
 * Output: "[SAFEZONE] Detected MAC: AA:BB:CC:DD:EE:FF"
 */
void safezone_log_mac(const char *mac_str);

/**
 * @brief Logs generated BLE device name at boot.
 * Output: "[SAFEZONE] Generated BLE Name: FirmGen_XX"
 */
void safezone_log_ble_name(const char *ble_name);

/**
 * @brief Logs BLE advertising status.
 * Output: "[SAFEZONE] BLE Advertising Started: FirmGen_XX"
 */
void safezone_log_ble_adv_started(const char *ble_name);

/**
 * @brief Logs perception state transition triggered by PipeGen.
 * Output: "[SAFEZONE] State -> HAZARD_ALERT | Zone: %s | Reason: %s | GPIO2=HIGH GPIO4=HIGH"
 */
void safezone_log_state_alert(const char *zone, const char *reason);

/**
 * @brief Logs state transition back to SAFE.
 * Output: "[SAFEZONE] State -> SAFE | GPIO2=LOW GPIO4=LOW"
 */
void safezone_log_state_safe(const char *trigger);

#ifdef __cplusplus
}
#endif

#endif // SAFEZONE_LOG_H
