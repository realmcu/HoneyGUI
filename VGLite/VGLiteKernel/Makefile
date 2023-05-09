
OBJS = linux/vg_lite_hal.o vg_lite_kernel.o linux/platform/$(PLATFORM).o

ifeq ($(DEBUG), 1)
EXTRA_CFLAGS += -g -O1 -DDEBUG -D_DEBUG
else
EXTRA_CFLAGS += -O2
endif

ifeq ($(ENABLE_PCIE), 1)
EXTRA_CFLAGS += -DENABLE_PCIE
endif

ifeq ($(gcdIRQ_SHARED), 1)
EXTRA_CFLAGS += -DgcdIRQ_SHARED
endif

ifeq ($(USE_RESERVE_MEMORY), 1)
EXTRA_CFLAGS += -DUSE_RESERVE_MEMORY
endif

ifeq ($(BACKUP_COMMAND), 1)
EXTRA_CFLAGS += -DBACKUP_COMMAND
endif

ifeq ($(KERNELRELEASE),)

ARCH ?= arm
KERNEL_DIR ?= /opt/linux-2.6.38-imx
CROSS_COMPILE ?= ~/CodeSourcery/bin/arm-none-linux-gnueabi-
PWD = $(shell pwd)
OUT = ../bin

all: $(OUT)
	@$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) V=$(V) modules
	@cp vg_lite.ko $(OUT)

clean:
	@rm -fr $(OBJS) *.o vg_lite.ko .*.cmd modules.order Module.symvers vg_lite.mod.c .tmp_versions $(OUT)/vg_lite.ko
	@rm -f ./linux/.*.cmd

$(OUT):
	@mkdir -p $(OUT)

else

obj-m := vg_lite.o
vg_lite-objs := $(OBJS)

EXTRA_CFLAGS += -I$(M)/../inc -I$(M)/linux -I$(M) -I$(M)/../VGLite/linux

endif
