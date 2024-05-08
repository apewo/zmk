/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <zmk/display/widgets/split_mode_status.h>
#include <zmk/split/service.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

int zmk_widget_split_mode_status_init(struct zmk_widget_split_mode_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    if(split_transport_mode == ZMK_SPLIT_MODE_BLUETOOTH){
        lv_label_set_text(widget->obj, "Wireless");
    } 
    if(split_transport_mode == ZMK_SPLIT_MODE_SERIAL){
        lv_label_set_text(widget->obj, "Serial");
    } 
    sys_slist_append(&widgets, &widget->node);

    return 0;
}

lv_obj_t *zmk_widget_split_mode_status_obj(struct zmk_widget_split_mode_status *widget) {
    return widget->obj;
}

