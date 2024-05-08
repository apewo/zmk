/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define ZMK_SERIAL_IS_CENTRAL                                                                         \
    (IS_ENABLED(CONFIG_ZMK_SPLIT) && IS_ENABLED(CONFIG_ZMK_SPLIT_SERIAL) &&                                 \
     IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL))

