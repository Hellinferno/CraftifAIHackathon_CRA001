#ifndef BLE_NAME_FMT_H
#define BLE_NAME_FMT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Extracts the last 2 uppercase hexadecimal digits from a 6-byte MAC address.
 *
 * Implements FR-003: Hex Suffix Extraction.
 *
 * @param[in] mac 6-byte MAC array.
 * @param[out] suffix_out Buffer of at least 3 bytes (2 hex digits + null terminator).
 * @param[in] max_len Buffer size.
 * @return true on success.
 */
bool extract_mac_suffix(const uint8_t mac[6], char *suffix_out, size_t max_len);

/**
 * @brief Builds the dynamic BLE advertised device name in the format "FirmGen_XX".
 *
 * Implements FR-004: Dynamic BLE Device Name Generation.
 *
 * @param[in] mac 6-byte MAC array.
 * @param[out] name_out Buffer of at least 12 bytes ("FirmGen_XX" + null terminator).
 * @param[in] max_len Buffer size.
 * @return true on success.
 */
bool build_firmgen_ble_name(const uint8_t mac[6], char *name_out, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif // BLE_NAME_FMT_H
