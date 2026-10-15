# ESP32 SafeZone-RT Closed-Loop BLE & Hazard Sentinel — PRD

**Target Platform:** ESP32 DevKit (ESP-IDF v5.5)  
**Target Tool:** [CraftifAI FirmGen v0.4.1](https://craftifai.com/products/firmgen) ([Developer Portal](https://developer.craftifai.com/))  
**Reference Specification:** Modeled on the official [ESP32 BLE Broadcast Simple PRD](https://drive.google.com/file/d/1T6PVA_S-QZf1Z_Zkv1YoGlz_7HVeuOF-/view?usp=sharing) and [Official Problem Statement](https://docs.google.com/document/d/1nxSV7QiLeO3DvOLIdt2AOiZQtMqIYqt5m2tSAqqwmAY/edit?usp=sharing)

*(Instructions: In FirmGen v0.4.1, select **Draft PRD → I have a document** and upload this file directly to generate the Explicit requirements, Task Lists, Acceptance Criteria, ESP32 Firmware, Unit Tests, and Traceability Report for the **Closed Loop** and **Live Intelligence** bonus prizes.)*

## Objective

Create requirements-driven firmware for an ESP32 DevKit that starts BLE advertising automatically after boot under the dynamic device name `FirmGen_XX` and acts as a real-time closed-loop physical safety actuator driven by perception alerts from a CraftifAI PipeGen computer vision pipeline over UART at 115200 baud.

## BLE Device Name & Hazard State Advertising

The BLE advertised device name shall be generated dynamically at runtime in the format:

`FirmGen_XX`

`XX` shall be the last 2 uppercase hexadecimal digits of the ESP32 MAC address.
* **Example:** If the detected MAC address is `A4:CF:12:34:56:9B`, the BLE advertised device name shall be `FirmGen_9B`.
* The BLE advertisement payload shall remain active continuously across both `SAFE` and `HAZARD_ALERT` states so mobile BLE scanners and nearby worker wearables can detect the sentinel at all times.

## Explicit Requirements

1. **FR-001 (BLE Initialization):** Initialize the ESP32 BLE stack (NimBLE or Bluedroid under ESP-IDF v5.5) automatically after device boot.
2. **FR-002 (Runtime MAC Read):** Read the ESP32 base/BLE MAC address dynamically at runtime using ESP-IDF system APIs.
3. **FR-003 (Hex Suffix Extraction):** Extract the last 2 hexadecimal digits of the detected MAC address in uppercase format (`%02X`).
4. **FR-004 (Dynamic BLE Name Generation):** Generate the BLE device name dynamically as `FirmGen_XX`, where `XX` is the extracted 2-digit hexadecimal suffix.
5. **FR-005 (Continuous BLE Advertising):** Start BLE advertising automatically using the generated name `FirmGen_XX` and keep BLE advertising continuously without dropping visibility.
6. **FR-006 (Closed-Loop UART Perception Command Parser):** Configure UART0 at `115200` baud (8N1) to receive newline-terminated perception event commands emitted by the PipeGen vision pipeline bridge:
   * `ALERT:<ZONE>:<REASON>` (for example, `ALERT:Restricted_Zone_A:NO-HARDHAT`) shall immediately transition the system state to `HAZARD_ALERT`.
   * `CLEAR` shall transition the system state back to `SAFE`.
7. **FR-007 (Physical Hazard Actuation):** Drive the onboard status/warning LED (`GPIO 2`) and optional external buzzer/relay pin (`GPIO 4`) according to the current closed-loop state:
   * In the `SAFE` state, keep `GPIO 2` and `GPIO 4` driven `LOW` (`0`).
   * In the `HAZARD_ALERT` state, drive `GPIO 2` and `GPIO 4` `HIGH` (`1`) within `50 ms` of receiving an `ALERT:` command from PipeGen.
   * Implement a hardware failsafe timeout that automatically resets the state to `SAFE` if `10 seconds` elapse without a renewed `ALERT:` command or if a `CLEAR` command is received.
8. **FR-008 (Deterministic UART Telemetry & Evidence Logging):** Use UART at `115200` baud for structured debug and acceptance verification logs:
   * Print the detected MAC address at boot (`[SAFEZONE] Detected MAC: AA:BB:CC:DD:EE:FF`).
   * Print the generated BLE device name at boot (`[SAFEZONE] Generated BLE Name: FirmGen_FF`).
   * Print a confirmation message when BLE advertising starts (`[SAFEZONE] BLE Advertising Started: FirmGen_FF`).
   * Print a timestamped state-transition message whenever a PipeGen perception command triggers `HAZARD_ALERT` or `SAFE` (`[SAFEZONE] State -> HAZARD_ALERT | Zone: Restricted_Zone_A | Reason: NO-HARDHAT | GPIO2=HIGH`).

## Acceptance Criteria

| Criterion ID | Linked Requirement(s) | Verification Method | Acceptance Condition |
| :--- | :--- | :--- | :--- |
| **AC-001** | `FR-001`, `FR-005`, `FR-008` | Target UART Monitor (`115200` baud) | The ESP32 boots cleanly without panic/reboot loops and prints the detected MAC address, the generated `FirmGen_XX` name, and the `BLE Advertising Started` message in order. |
| **AC-002** | `FR-002`, `FR-003`, `FR-004` | Host Unit Test & Target UART Monitor | The advertised BLE name matches `FirmGen_` followed by the exact last 2 uppercase hexadecimal digits of the ESP32 MAC address. |
| **AC-003** | `FR-005` | Mobile BLE Scanner App (On-Target) | The device `FirmGen_XX` is visible in a mobile BLE scanner application and remains continuously discoverable. |
| **AC-004** | `FR-006`, `FR-007`, `FR-008` | Closed-Loop HIL / Target Monitor & GPIO Inspection | Sending `ALERT:Restricted_Zone_A:NO-HARDHAT` from the PipeGen bridge sets `GPIO 2` (`LED`) and `GPIO 4` `HIGH` within `50 ms` and prints the `State -> HAZARD_ALERT` log line on UART; sending `CLEAR` sets `GPIO 2` and `GPIO 4` `LOW` and prints `State -> SAFE`. |

## FirmGen Review & Freeze Checklist (Step 5 in FirmGen v0.4.1)

When FirmGen finishes generating requirements from this PRD, follow the rules from the [FirmGen v0.4.1 Hands-On Lab](https://drive.google.com/file/d/1O1T4MvvToPP5rbnfR8O4Ymf4qg4haU5g/view?usp=sharing):
* **Confirm Explicit Requirements (`FR-001` through `FR-008`):** Ensure every requirement above is tagged **Explicit** and retains its exact `FirmGen_XX` formatting and `115200` baud UART specification.
* **Prune Out-of-Scope Inferred Requirements:** Reject any inferred requirements that add GATT pairing, bonding, encrypted BLE data connections, or Wi-Fi cloud provisioning so Auto Mode implementation stays fast, deterministic, and strictly focused on the PRD baseline.
* **Enable Live Intelligence (Bonus Prize):** After Auto Mode builds and flashes the firmware, open the **Live Intelligence** panel on the right side of the FirmGen workspace, select **Enable Telemetry**, and display the RTOS / System Monitor alongside the UART output during judging.

## Requirement-to-Implementation Traceability Matrix Reference

FirmGen automatically generates a traceability report in `sources/` linking each requirement to its acceptance criteria, C implementation modules, and Ceedling/Unity unit tests. Use this table in your submission documentation to highlight end-to-end traceability:

| Requirement ID | Category | Tag | Target Firmware Module & Symbol | Unit Test / Verification Artifact | Verification Method |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **FR-001** | BLE Subsystem | Explicit | `firmware/modules/ble_adv/ble_adv.c` (`ble_adv_init`) | `test_FR_001_ble_init.c` | Target Monitor & BLE Scanner (`AC-001`) |
| **FR-002** | Device Identity | Explicit | `firmware/drivers/mac_id/mac_reader.c` (`mac_read_base`) | `test_FR_002_mac_read.c` | Target Monitor (`AC-001`, `AC-002`) |
| **FR-003** | Device Identity | Explicit | `firmware/modules/ble_name/ble_name_fmt.c` (`extract_mac_suffix`) | `test_FR_003_mac_suffix.c` | Host Unit Test (`AC-002`) |
| **FR-004** | Device Identity | Explicit | `firmware/modules/ble_name/ble_name_fmt.c` (`build_firmgen_ble_name`) | `test_FR_004_name_format.c` | Host Unit Test & Target (`AC-002`) |
| **FR-005** | BLE Subsystem | Explicit | `firmware/modules/ble_adv/ble_adv.c` (`ble_adv_start_continuous`) | `test_FR_005_ble_continuous.c` | Mobile BLE Scanner (`AC-003`) |
| **FR-006** | Closed-Loop Bridge | Explicit | `firmware/modules/cmd_parser/uart_cmd_parser.c` (`parse_pipegen_frame`) | `test_FR_006_cmd_parser.c` | Host Unit Test & HIL (`AC-004`) |
| **FR-007** | Physical Actuation | Explicit | `firmware/bsp/gpio_actuator.c` (`actuator_set_hazard_state`) | `test_FR_007_gpio_actuation.c` | Target GPIO & Watchdog Test (`AC-004`) |
| **FR-008** | Telemetry & UART | Explicit | `firmware/modules/logger/safezone_log.c` (`safezone_uart_log`) | `test_FR_008_uart_logging.c` | UART Monitor @ 115200 (`AC-001`, `AC-004`) |
