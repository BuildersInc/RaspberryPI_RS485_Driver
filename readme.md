# RS485 GPIO Driver for Raspberry pi 4

This is a GPIO driver to enable communication between the PI and
a connected MAX485 chip.

The particular board uses two MAX485 chips to enable two different channels.

The Pinout is as follows

| Channel   |        A       |        B       |
|:---------:|:--------------:|:--------------:|
| TX        | GPIO14 / PIN08 | ID_SDA / PIN27 |
| RX        | GPIO15 / Pin10 | ID_SCL / PIN28 |
| DE        | GPIO27 / PIN13 | GPIO05 / PIN29 |
| RE_N      | GPIO22 / PIN15 | GPIO06 / PIN31 |
