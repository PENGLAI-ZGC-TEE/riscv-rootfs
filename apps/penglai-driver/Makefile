##
## Author: Dong Du
## Description:
## 	The file now assumes the kernel located in ../openeuler-kernel,
##	if you would like to use your own one, please change the path

obj-m += penglai.o
penglai-objs := penglai-enclave-driver.o \
	penglai-enclave-elfloader.o \
	penglai-enclave-page.o \
	penglai-enclave.o \
	penglai-enclave-ioctl.o 

LINUX_HOME=/nfs/home/pengxuanyao/Workspace/NEMU-TEE/riscv-linux

all:
	make -C $(LINUX_HOME) ARCH=riscv M=$(shell pwd) CROSS_COMPILE=riscv64-unknown-linux-gnu- modules
	-mkdir $(RISCV_ROOTFS_HOME)/rootfsimg/build/
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/penglai.ko
	ln -sf $(abspath ./penglai.ko) $(RISCV_ROOTFS_HOME)/rootfsimg/build/penglai.ko

clean:
	make -C $(LINUX_HOME) ARCH=riscv M=$(PWD) clean
