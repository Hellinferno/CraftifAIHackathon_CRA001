#ifndef GPIO_ACTUATOR_H
#define GPIO_ACTUATOR_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ACTUATOR_PIN_LED     2  // Onboard warning LED (GPIO 2)
#define ACTUATOR_PIN_BUZZER  4  // External buzzer / strobe relay (GPIO 4)
#define FAILSAFE_TIMEOUT_SEC 10 // Hardware failsafe automatic reset window (10s)

typedef enum {
    SYSTEM_STATE_SAFE = 0,
    SYSTEM_STATE_HAZARD_ALERT,
} system_hazard_state_t;

/**
 * @brief Configures GPIO 2 and GPIO 4 as push-pull outputs initialized to LOW.
 *
 * Implements FR-007: Physical Hazard Actuation Setup.
 *
 * @return true on success.
 */
bool actuator_init(void);

/**
 * @brief Sets the physical closed-loop actuation state.
 *
 * - In SAFE: drives GPIO 2 and GPIO 4 LOW (0).
 * - In HAZARD_ALERT: drives GPIO 2 and GPIO 4 HIGH (1) within 50ms.
 *
 * Implements FR-007: Deterministic Actuation (<50ms response).
 *
 * @param[in] state Target state (SYSTEM_STATE_SAFE or SYSTEM_STATE_HAZARD_ALERT).
 */
void actuator_set_hazard_state(system_hazard_state_t state);

/**
 * @brief Returns the current physical actuation state.
 */
system_hazard_state_t actuator_get_current_state(void);

/**
 * @brief Checks and handles the 10-second failsafe watchdog.
 *
 * If 10 seconds elapse without a renewed ALERT command, resets to SAFE.
 *
 * @param[in] current_time_ms Current system timestamp in milliseconds.
 * @return true if state was automatically reset to SAFE by failsafe timeout.
 */
bool actuator_failsafe_tick(uint32_t current_time_ms);

#ifdef __cplusplus
}
#endif

#endif // GPIO_ACTUATOR_H
