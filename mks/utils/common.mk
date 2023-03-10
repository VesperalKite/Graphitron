FPGA_PATH = ./fpga_accelerator
FPGA_APP_PATH = $(FPGA_PATH)/$(APP)
FPGA_KERNEL_PATH = $(FPGA_PATH)/kernel

XCLBIN := ./xclbin_$(APP)
HOST_ARCH := x86

#Check for g++
# ifeq ($(HOST_ARCH), x86)
# ifneq ($(shell expr $(shell g++ -dumpversion) \>= 5), 1)
# ifndef XILINX_VITIS
# $(error [ERROR]: g++ version older. Please use 5.0 or above.)
# else
# CXX := $(XILINX_VIVADO)/tps/lnx64/gcc-6.2.0/bin/g++
# $(warning [WARNING]: g++ version older. Using g++ provided by the tool : $(CXX))
# endif
# endif
# else ifeq ($(HOST_ARCH), aarch64)
# CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++
# else ifeq ($(HOST_ARCH), aarch32)
# CXX := $(XILINX_VITIS)/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-g++
# else
# $(error [ERROR]: Please check host architecture set.)
# endif

XOCC := v++

HOST_SRCS = $(FPGA_APP_PATH)/main.cpp