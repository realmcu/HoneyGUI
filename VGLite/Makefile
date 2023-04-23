SDK_DIR ?= ./build
.phony: all install doc

all:
ifneq ($(KERNEL_DIR),)
	@$(MAKE) -C VGLiteKernel
endif
	@$(MAKE) -C VGLite

clean:
	@rm -rf bin
ifneq ($(KERNEL_DIR),)
	@$(MAKE) -C VGLiteKernel clean
endif
	@$(MAKE) -C VGLite clean

install: all $(SDK_DIR)
	@cp -rf bin/* $(SDK_DIR)/drivers
	@cp -f inc/* $(SDK_DIR)/inc

$(SDK_DIR):
	@mkdir -p $(SDK_DIR)/drivers
	@mkdir -p $(SDK_DIR)/inc
