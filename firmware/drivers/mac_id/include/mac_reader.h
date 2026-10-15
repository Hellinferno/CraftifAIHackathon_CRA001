#ifndef MAC_READER_H
#define MAC_READER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reads the 6-byte base MAC address from the ESP32 hardware eFuse.
 *
 * Implements FR-002: Dynamic Runtime MAC Reading.
 *
 * @param[out] mac 6-byte array to store the detected MAC address.
 * @return true if MAC was successfully read, false otherwise.
 */
bool mac_read_base(uint8_t mac[6]);

/**
 * @brief Formats a 6-byte MAC address into standard AA:BB:CC:DD:EE:FF representation.
 *
 * @param[in] mac 6-byte MAC array.
 * @param[out] out_str Buffer of at least 18 bytes.
 * @param[in] max_len Size of out_str buffer.
 * @return true on success.
 */
bool mac_to_string(const uint8_t mac[6], char *out_str, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif // MAC_READER_H
