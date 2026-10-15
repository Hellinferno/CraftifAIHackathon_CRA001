#include "gpio_actuator.h"
#include <stdio.h>

#if defined(ESP_PLATFORM)
#include "driver/gpio.h"
#include "esp_timer.h"
#endif

static system_hazard_state_t s_current_state = SYSTEM_STATE_SAFE;
static uint32_t s_last_alert_time_ms = 0;

bool actuator_init(void) {
#if defined(ESP_PLATFORM)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << ACTUATOR_PIN_LED) | (1ULL << ACTUATOR_PIN_BUZZER),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    if (gpio_config(&io_conf) != ESP_OK) {
        return false;
    }
    gpio_set_level(ACTUATOR_PIN_LED, 0);
    gpio_set_level(ACTUATOR_PIN_BUZZER, 0);
#endif
    s_current_state = SYSTEM_STATE_SAFE;
    s_last_alert_time_ms = 0;
    return true;
}

void actuator_set_hazard_state(system_hazard_state_t state) {
    s_current_state = state;
    int pin_level = (state == SYSTEM_STATE_HAZARD_ALERT) ? 1 : 0;

#if defined(ESP_PLATFORM)
    gpio_set_level(ACTUATOR_PIN_LED, pin_level);
    gpio_set_level(ACTUATOR_PIN_BUZZER, pin_level);
    if (state == SYSTEM_STATE_HAZARD_ALERT) {
        s_last_alert_time_ms = (uint32_t)(esp_timer_get_time() / 1000);
    }
#else
    (void)pin_level;
#endif
}

system_hazard_state_t actuator_get_current_state(void) {
    return s_current_state;
}

bool actuator_failsafe_tick(uint32_t current_time_ms) {
    if (s_current_state == SYSTEM_STATE_HAZARD_ALERT) {
        if (current_time_ms >= s_last_alert_time_ms &&
            (current_time_ms - s_last_alert_time_ms) > (FAILSAFE_TIMEOUT_SEC * 1000)) {
            actuator_set_hazard_state(SYSTEM_STATE_SAFE);
            return true;
        }
    }
    return false;
}
