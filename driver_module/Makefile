obj-m +=hello.o


# 使用整个编译生成的文件来编译
KERNELDIR ?= /data/caohang/work/amlogic/repo-02/ott/aml_02/build/base/out/target/product/p201_iptv/obj/KERNEL_OBJ

PWD?=$(shell pwd)
ARCH = arm64

# 自己下载的交叉编译工具的位置
CROSS_COMPILE=/data/caohang/work/amlogic/repo-02/chongqing/build/base/mine_driver/arch/bin/aarch64-linux-android-

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
CFLAGS_MODULE=-fno-pic -Werror=strict-prototypes

.PHONY:modules clean
modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $ $(KERNELDIR) M=$(PWD) modules
clean:
	rm *.o *.mod.c *.order *.symvers
