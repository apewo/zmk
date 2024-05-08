/*
 * Copyright (c) 2023 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/matrix.h>

#include <zmk/split/service.h>
#include <zmk/split/serial/serial.h>
#include <zmk/split/service.h>

// TODO TODO TODO
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(slicemk);

static void serial_handle_run_behavior(void *data, uint8_t len) {
    // LOG_HEXDUMP_ERR(data, len, "central to peripheral");

    if (len != sizeof(struct zmk_split_run_behavior_payload)) {
        LOG_ERR("wrong length received in serial_handle_run_behavior : %u", len);
        return;
    }

    struct zmk_split_run_behavior_payload *payload = data;

    size_t behavior_dev_len = strlen(payload->behavior_dev);
    if(behavior_dev_len >=  ZMK_SPLIT_RUN_BEHAVIOR_DEV_LEN){
        LOG_ERR("wrong length received in serial_handle_run_behavior : %u", behavior_dev_len);
        return;
    }
    struct zmk_behavior_binding binding = {
        .param1 = payload->data.param1,
        .param2 = payload->data.param2,
        .behavior_dev = payload->behavior_dev,
    };
    LOG_DBG("%s with params %d %d: pressed? %d", binding.behavior_dev, binding.param1,
            binding.param2, payload->data.state);
    struct zmk_behavior_binding_event event = {.position = payload->data.position,
                                               .timestamp = k_uptime_get()};
    int err;
    if (payload->data.state > 0) {
        err = behavior_keymap_binding_pressed(&binding, event);
    } else {
        err = behavior_keymap_binding_released(&binding, event);
    }

    if (err) {
        LOG_ERR("Failed to invoke behavior %s: %d", binding.behavior_dev, err);
    }
}

void serial_handle_rx(uint32_t cmd, uint8_t *data, uint8_t len) {
    switch (cmd) {
    //peripheral run behavior v0 (prb\00)
    case 0x70726200:
        serial_handle_run_behavior(data, len);
        break;

    default:
        LOG_ERR("Received unexpected UART command 0x%08x", cmd);
        break;
    }
}

void send_position_state_impl_serial(uint8_t *state, int len) {
    serial_write_uart(0x73627400, state, len);
}
