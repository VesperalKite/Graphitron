SHELL := /bin/bash
TARGET := $(TARGETS)
DEVICE := $(DEVICES)

COMMON_REPO     = ./
ABS_COMMON_REPO = $(shell readlink -f $(COMMON_REPO))
UTILS_PATH      = ./utils
ROOT_REPO = ../../
ABS_ROOT_REPO = $(shell readlink -f $(ROOT_REPO))
FPGA_PATH = ./fpga_accelerator
FPGA_KERNEL_PATH = $(FPGA_PATH)/kernel


include ${UTILS_PATH}/help.mk

.PHONY: all clean build

all: precheck
clean: precheck

precheck:
ifndef app
	$(error app is undefined)
else
APP = $(app)
APP_INPUT = ./apps/$(APP).gn
FPGA_APP_PATH = $(FPGA_PATH)/$(APP)

include ${UTILS_PATH}/utils.mk

include $(FPGA_APP_PATH)/config.mk
include $(FPGA_APP_PATH)/build.mk

include $(UTILS_PATH)/common.mk

include $(FPGA_KERNEL_PATH)/gs_kernel.mk
include $(FPGA_KERNEL_PATH)/apply_kernel.mk

include ${UTILS_PATH}/compile.mk
include ${UTILS_PATH}/clean.mk

compile: build $(BINARY_CONTAINERS)

exe: $(EXECUTABLE)

all: compile exe

endif