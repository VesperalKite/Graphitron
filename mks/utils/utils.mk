PROFILE := no

ifeq ($(PROFILE), yes)
LDCLFLGAS += --profile_kernel data:all:all:all
endif

DEBUG := no

ifeq ($(DEBUG), yes)
CLFLAGS += --dk protocol:all:all:all
LDCLFLGAS += --dk list_ports
endif

ifndef XILINX_VITIS
$(error XILINX_VITIS variable is not set, Please set correctly and rerun)
endif

RM = rm -f
RMDIR = rm -rf
MK = mkdir
MV = mv
CP = cp -r
VAR_TRUE = true