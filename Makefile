# sudo apt install raspberrypi-kernel-headers
# the makefile has to be named "Makefile"

obj-m += src/raspi_rs485.o

KDIR = /lib/modules/$(shell uname -r)/build

PWD = $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
