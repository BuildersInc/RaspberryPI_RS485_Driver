# sudo apt install raspberrypi-kernel-headers
# the makefile has to be named "Makefile"

KERNEL = $(shell uname -r)
# KERNEL = 6.1.0-rpi7-rpi-v7

# obj-m += src/raspi_rs485.o
obj-m += raspi.o

raspi-objs := src/raspi_rs485.o src/logger.o src/raspi_gpio.o

KDIR = /lib/modules/$(KERNEL)/build

PWD = $(shell pwd)

all:
	rmmod raspi | true
	make -C $(KDIR) M=$(PWD) modules
	insmod raspi.ko

clean:
	rmmod raspi | true
	make -C $(KDIR) M=$(PWD) clean
