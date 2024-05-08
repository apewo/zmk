/*
 * Copyright (c) 2023 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */


#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zmk/split/service.h>

LOG_MODULE_REGISTER(split_init, CONFIG_ZMK_LOG_LEVEL);

enum zmk_split_transport_mode split_transport_mode= ZMK_SPLIT_MODE_NONE;

static int zmk_split_init(void) {


//TODO: not tested
#if defined(CONFIG_ZMK_SPLIT_BLE) && !defined(CONFIG_ZMK_SPLIT_SERIAL)
    split_transport_mode= ZMK_SPLIT_MODE_BLUETOOTH;
//TODO: not tested
#elif !defined(CONFIG_ZMK_SPLIT_BLE) && defined(CONFIG_ZMK_SPLIT_SERIAL)
    split_transport_mode= ZMK_SPLIT_MODE_SERIAL;
#else
    split_transport_mode= ZMK_SPLIT_MODE_SERIAL;
    LOG_INF("starting split init module");


    const struct device *gpio_dev = device_get_binding(CONFIG_ZMK_SPLIT_INIT_MODE_GPIO_LABEL);
    if (gpio_dev == NULL) {
         LOG_ERR("gpio_dev is null");
        return;
    }

/* TODO TODO TODO
 atm one need :

&gpio0 {
    label = "GPIO_0";
    status = "okay";
};


CONFIG_ZMK_SPLIT_INIT_MODE_GPIO_PULL_UP=y
CONFIG_ZMK_SPLIT_INIT_MODE_GPIO_PIN=10
CONFIG_ZMK_SPLIT_INIT_MODE_GPIO_LABEL="GPIO_0"

 we could get rid of the Kconfig params and declare a button like this and use DT_XXX macros to retrieve conf ???
    aliases {
         bootswitch = &boot_switch;
     };

     buttons {
         compatible = "gpio-keys";
         boot_switch: boot_switch {
             gpios = <&gpio0 10 (GPIO_PULL_UP | GPIO_ACTIVE_LOW)>;
             label = "split_transport_mode_switch";
         };
     };
 */

#if defined(CONFIG_ZMK_SPLIT_INIT_MODE_GPIO_PULL_UP)
#define ZMK_GPIO_PULL_MODE GPIO_PULL_UP
#else
#define ZMK_GPIO_PULL_MODE GPIO_PULL_DOWN
#endif
    //TODO: put GPIO_ACTIVE_LOW as kconfig param 
    int  ret = gpio_pin_configure(gpio_dev, CONFIG_ZMK_SPLIT_INIT_MODE_GPIO_PIN , GPIO_INPUT | ZMK_GPIO_PULL_MODE | GPIO_ACTIVE_LOW );
    if (ret < 0) {
        LOG_ERR("could not configure gpio pin : %d", ret);
        return ret;
    }

    int gpio_state = gpio_pin_get(gpio_dev, CONFIG_ZMK_SPLIT_INIT_MODE_GPIO_PIN);
    if (gpio_state < 0) {
        LOG_ERR("could not get gpio state : %d", gpio_state);
        return gpio_state;
    }

    LOG_INF("read split init gpio state : %d", gpio_state);
    if (gpio_state == 0) { // Active low means button pressed
        split_transport_mode= ZMK_SPLIT_MODE_SERIAL;
    }else{
        split_transport_mode= ZMK_SPLIT_MODE_BLUETOOTH;
    }
     ret = gpio_pin_configure(gpio_dev, CONFIG_ZMK_SPLIT_INIT_MODE_GPIO_PIN, GPIO_DISCONNECTED);
     if (ret < 0) {
         // split_transport_mode= ZMK_SPLIT_MODE_SERIAL;
         LOG_ERR("could not disconnect gpio pin : %d", ret);
         return ret;
     }

        LOG_INF("end split init module");
#endif
    return 0;

}

//PRE_KERNEL_1 & LOG_XXX wont work on usb logging , use a later level
// SYS_INIT(zmk_split_init, PRE_KERNEL_1, 0);
SYS_INIT(zmk_split_init, APPLICATION, 30);
