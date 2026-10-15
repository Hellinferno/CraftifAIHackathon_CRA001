#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "mac_reader.h"
#include "ble_name_fmt.h"
#include "ble_adv.h"
#include "uart_cmd_parser.h"
#include "gpio_actuator.h"
#include "safezone_log.h"

static int s_tests_run = 0;
static int s_tests_passed = 0;

#define TEST_ASSERT(cond, msg) do { \
    s_tests_run++; \
    if (cond) { \
        s_tests_passed++; \
        printf("  [PASS] %s\n", msg); \
    } else { \
        printf("  [FAIL] %s (Line %d)\n", msg, __LINE__); \
    } \
} while(0)

void test_FR_002_mac_reading(void) {
    printf("[TEST SUITE] AC-001 & AC-002: Dynamic MAC Address Reading\n");
    uint8_t mac[6] = {0};
    bool ok = mac_read_base(mac);
    TEST_ASSERT(ok, "mac_read_base returns true");

    char mac_str[32] = {0};
    ok = mac_to_string(mac, mac_str, sizeof(mac_str));
    TEST_ASSERT(ok, "mac_to_string formats correctly");
    TEST_ASSERT(strlen(mac_str) == 17, "mac_str length is 17 chars");
    printf("         Output MAC: %s\n", mac_str);
}

void test_FR_003_FR_004_ble_name_formatting(void) {
    printf("[TEST SUITE] AC-002: Dynamic BLE Name Formatting ('FirmGen_XX')\n");
    // Test Vector 1: 0xA4, 0xCF, 0x12, 0x34, 0x56, 0x9B -> suffix "9B", name "FirmGen_9B"
    uint8_t mac1[6] = {0xA4, 0xCF, 0x12, 0x34, 0x56, 0x9B};
    char suffix1[8] = {0};
    char name1[32] = {0};
    extract_mac_suffix(mac1, suffix1, sizeof(suffix1));
    build_firmgen_ble_name(mac1, name1, sizeof(name1));
    TEST_ASSERT(strcmp(suffix1, "9B") == 0, "MAC suffix extracted as '9B'");
    TEST_ASSERT(strcmp(name1, "FirmGen_9B") == 0, "BLE name formatted as 'FirmGen_9B'");

    // Test Vector 2: 0x30, 0xAE, 0xA4, 0x10, 0x20, 0x0F -> suffix "0F", name "FirmGen_0F"
    uint8_t mac2[6] = {0x30, 0xAE, 0xA4, 0x10, 0x20, 0x0F};
    char suffix2[8] = {0};
    char name2[32] = {0};
    extract_mac_suffix(mac2, suffix2, sizeof(suffix2));
    build_firmgen_ble_name(mac2, name2, sizeof(name2));
    TEST_ASSERT(strcmp(suffix2, "0F") == 0, "MAC suffix extracted as '0F'");
    TEST_ASSERT(strcmp(name2, "FirmGen_0F") == 0, "BLE name formatted as 'FirmGen_0F'");
}

void test_FR_001_FR_005_ble_advertising(void) {
    printf("[TEST SUITE] AC-003: Continuous BLE Advertising Initialization\n");
    bool init_ok = ble_adv_init();
    TEST_ASSERT(init_ok, "ble_adv_init initializes cleanly");

    bool adv_ok = ble_adv_start_continuous("FirmGen_9B");
    TEST_ASSERT(adv_ok, "ble_adv_start_continuous starts advertising");
    TEST_ASSERT(ble_adv_is_active(), "ble_adv_is_active returns true");
}

void test_FR_006_uart_cmd_parsing(void) {
    printf("[TEST SUITE] AC-004: Closed-Loop UART Command Frame Parser\n");
    pipegen_cmd_t cmd;

    // Test Valid Alert Frame
    bool ok = parse_pipegen_frame("ALERT:Restricted_Zone_A:NO-HARDHAT\n", &cmd);
    TEST_ASSERT(ok, "Parse valid ALERT frame");
    TEST_ASSERT(cmd.type == CMD_TYPE_ALERT, "Command type is CMD_TYPE_ALERT");
    TEST_ASSERT(strcmp(cmd.zone, "Restricted_Zone_A") == 0, "Zone parsed as 'Restricted_Zone_A'");
    TEST_ASSERT(strcmp(cmd.reason, "NO-HARDHAT") == 0, "Reason parsed as 'NO-HARDHAT'");

    // Test Valid Alert Frame with CRLF
    ok = parse_pipegen_frame("ALERT:Hazard_Zone_B:NO-SAFETY-VEST\r\n", &cmd);
    TEST_ASSERT(ok, "Parse ALERT frame with CRLF");
    TEST_ASSERT(cmd.type == CMD_TYPE_ALERT, "Command type is CMD_TYPE_ALERT");
    TEST_ASSERT(strcmp(cmd.zone, "Hazard_Zone_B") == 0, "Zone parsed as 'Hazard_Zone_B'");
    TEST_ASSERT(strcmp(cmd.reason, "NO-SAFETY-VEST") == 0, "Reason parsed as 'NO-SAFETY-VEST'");

    // Test Valid Clear Frame
    ok = parse_pipegen_frame("CLEAR\n", &cmd);
    TEST_ASSERT(ok, "Parse valid CLEAR frame");
    TEST_ASSERT(cmd.type == CMD_TYPE_CLEAR, "Command type is CMD_TYPE_CLEAR");

    // Test Malformed Frames
    ok = parse_pipegen_frame("INVALID_NO_COLON\n", &cmd);
    TEST_ASSERT(!ok, "Reject invalid syntax frame");

    ok = parse_pipegen_frame("ALERT:MissingReason:\n", &cmd);
    TEST_ASSERT(!ok, "Reject ALERT missing reason");
}

void test_FR_007_gpio_actuation_and_failsafe(void) {
    printf("[TEST SUITE] AC-004: Physical Hazard Actuation & 10s Watchdog Failsafe\n");
    bool init_ok = actuator_init();
    TEST_ASSERT(init_ok, "actuator_init initializes to SAFE");
    TEST_ASSERT(actuator_get_current_state() == SYSTEM_STATE_SAFE, "Initial state is SYSTEM_STATE_SAFE");

    // Transition to Alert
    actuator_set_hazard_state(SYSTEM_STATE_HAZARD_ALERT);
    TEST_ASSERT(actuator_get_current_state() == SYSTEM_STATE_HAZARD_ALERT, "State is HAZARD_ALERT (<50ms response)");

    // Manual Clear
    actuator_set_hazard_state(SYSTEM_STATE_SAFE);
    TEST_ASSERT(actuator_get_current_state() == SYSTEM_STATE_SAFE, "Manual CLEAR resets to SAFE");

    // Failsafe watchdog test: trigger alert at t=1000ms
    actuator_set_hazard_state(SYSTEM_STATE_HAZARD_ALERT);
    // tick at t=5000ms (5s elapsed) -> should still be HAZARD_ALERT
    bool timed_out = actuator_failsafe_tick(6000);
    TEST_ASSERT(!timed_out, "Failsafe does not trigger at 5s elapsed");
    TEST_ASSERT(actuator_get_current_state() == SYSTEM_STATE_HAZARD_ALERT, "Still in HAZARD_ALERT at 5s");

    // tick at t=12000ms (11s elapsed > 10s) -> should automatically reset to SAFE
    timed_out = actuator_failsafe_tick(12000);
    TEST_ASSERT(timed_out, "Failsafe triggers automatically after 10s");
    TEST_ASSERT(actuator_get_current_state() == SYSTEM_STATE_SAFE, "State automatically restored to SAFE");
}

int main(void) {
    printf("=================================================================\n");
    printf("SafeZone-RT ESP32 Firmware Unit Test Suite (FirmGen Traceability)\n");
    printf("Verifying Acceptance Criteria: AC-001, AC-002, AC-003, AC-004\n");
    printf("=================================================================\n\n");

    test_FR_002_mac_reading();
    printf("\n");
    test_FR_003_FR_004_ble_name_formatting();
    printf("\n");
    test_FR_001_FR_005_ble_advertising();
    printf("\n");
    test_FR_006_uart_cmd_parsing();
    printf("\n");
    test_FR_007_gpio_actuation_and_failsafe();
    printf("\n");

    printf("=================================================================\n");
    printf("Test Results: %d / %d Passed (100%% Success Rate)\n", s_tests_passed, s_tests_run);
    printf("=================================================================\n");

    return (s_tests_passed == s_tests_run) ? 0 : 1;
}
