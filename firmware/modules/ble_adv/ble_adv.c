#include "ble_adv.h"
#include <stdio.h>
#include <string.h>

static bool s_ble_initialized = false;
static bool s_ble_advertising = false;
static char s_advertised_name[32] = {0};

#if defined(ESP_PLATFORM)
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

static void ble_app_on_sync(void) {
    int rc;
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (uint8_t *)s_advertised_name;
    fields.name_len = strlen(s_advertised_name);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        return;
    }

    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
                           &adv_params, NULL, NULL);
    if (rc == 0) {
        s_ble_advertising = true;
    }
}

static void host_task(void *param) {
    nimble_port_run();
    nimble_port_freertos_deinit();
}
#endif

bool ble_adv_init(void) {
#if defined(ESP_PLATFORM)
    esp_err_t ret = nimble_port_init();
    if (ret != ESP_OK) {
        return false;
    }
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task);
#endif
    s_ble_initialized = true;
    return true;
}

bool ble_adv_start_continuous(const char *device_name) {
    if (!device_name || strlen(device_name) == 0) return false;
    strncpy(s_advertised_name, device_name, sizeof(s_advertised_name) - 1);

#if defined(ESP_PLATFORM)
    ble_svc_gap_device_name_set(device_name);
#endif
    s_ble_advertising = true;
    return true;
}

bool ble_adv_is_active(void) {
    return s_ble_advertising;
}
