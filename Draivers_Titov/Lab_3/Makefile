obj-m += my_random.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o random_user random_user.c
	gcc -o entropier entropier.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f random_user entropier

install:
	sudo insmod my_random.ko

uninstall:
	sudo rmmod my_random

test: all install
	sudo ./random_user

.PHONY: all clean install uninstall test