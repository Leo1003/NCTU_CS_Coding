CC = gcc
PWD := $(shell pwd)
KVERSION := $(shell uname -r)
KERNEL_DIR ?= /lib/modules/$(KVERSION)/build
MODULE_NAME = page_table_driver
obj-m := $(MODULE_NAME).o
EXE = page_table server client

.PHONY: module install remove clean

all: $(EXE) module

$(EXE): %: %.c
	$(CC) -o $@ $^

module:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf $(EXE)

install: module
	insmod $(MODULE_NAME).ko
	mknod /dev/os c 200 0

remove:
	rm -rf /dev/os
	rmmod -f $(MODULE_NAME).ko
