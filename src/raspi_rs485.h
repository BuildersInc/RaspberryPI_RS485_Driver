#pragma once

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/mod_devicetable.h>
// #include <linux/proberty.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>


struct raspi_rs485
{
    /*GPIO PORT*/
    unsigned int TX_PIN;
    unsigned int RX_PIN;
    unsigned int DE_PIN;
    unsigned int RE_N_PIN;

    /*DRIVER DEVICE*/
    dev_t device_number;
    struct class* driver_class;
    struct cdev device;
    char* device_name;
    char* class_name;
};

