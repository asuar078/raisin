.PHONY: all
obj-m := rootkit.o
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
all: rootkit grape
rootkit:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD)
grape:
	gcc -o grape grape.c --std=gnu99 -Wall -Wextra -pedantic
clean:
	rm -rf *.o *.ko *.symvers *.mod.* *.order
