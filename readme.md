# RS485 GPIO Driver for Raspberry pi 4

This is a GPIO driver to enable communication between the PI and
a connected MAX485 chip.

The particular board uses two MAX485 chips to enable two different channels.

The Pinout is as follows

| Channel   |        A       |        B       |
|:---------:|:--------------:|:--------------:|
| TX        | GPIO14 / PIN08 | GPIO00 / PIN27 |
| RX        | GPIO15 / Pin10 | GPIO01 / PIN28 |
| DE        | GPIO27 / PIN13 | GPIO05 / PIN29 |
| RE_N      | GPIO22 / PIN15 | GPIO06 / PIN31 |


## Tutorial

```sh
# Install the driver
sudo insmod raspi_rs485.ko

# Remove the driver
sudo rmmod raspi_rs485

```
