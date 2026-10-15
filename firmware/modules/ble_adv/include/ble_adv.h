#ifndef BLE_ADV_H
#define BLE_ADV_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the BLE controller and host stack (NimBLE).
 *
 * Implements FR-001: BLE Stack Initialization.
 *
 * @return true on successful initialization.
 */
bool ble_adv_init(void);

/**
 * @brief Starts continuous non-connectable or connectable BLE advertising with device name.
 *
 * Implements FR-005: Continuous BLE Advertising.
 *
 * @param[in] device_name Null-terminated BLE advertised name (e.g., "FirmGen_9B").
 * @return true if advertising successfully started.
 */
bool ble_adv_start_continuous(const char *device_name);

/**
 * @brief Checks if BLE advertising is currently active.
 *
 * @return true if actively advertising.
 */
bool ble_adv_is_active(void);

#ifdef __cplusplus
}
#endif

#endif // BLE_ADV_H
