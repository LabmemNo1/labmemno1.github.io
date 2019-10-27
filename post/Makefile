obj-m	:=sem.o
CROSS_COMPILE    =arm-none-linux-gnueabi-
CC    	= $(CROSS_COMPILE)gcc
#CC	 	= gcc
KDIR		:=/work/nei/android-kernel-samsung-dev
#KDIR	:=/lib/modules/3.13.0-32-generic/build/
PWD	:=$(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf *.o* *~ core .depend .*.cmd *.ko *.mod.c
	rm -rf .tmp_versions/ 
	rm -rf *.symvers
