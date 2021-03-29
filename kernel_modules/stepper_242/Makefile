obj-m := stepper_242.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

modules_install:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules_install
	depmod -A

