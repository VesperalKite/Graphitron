XCLBIN := ./xclbin_$(APP)
HOST_ARCH := x86

##################################################
#												 #
#		        Compiler set up			    	 #
#												 #
##################################################

#Check for g++
ifeq ($(HOST_ARCH), x86)
ifneq ($(shell expr $(shell g++ -dumpversion) \>= 5), 1)
ifndef XILINX_VITIS
$(error [ERROR]: g++ version older. Please use 5.0 or above.)
else
CXX := $(XILINX_VIVADO)/tps/lnx64/gcc-6.2.0/bin/g++
$(warning [WARNING]: g++ version older. Using g++ provided by the tool : $(CXX))
endif
endif
else ifeq ($(HOST_ARCH), aarch64)
CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++
else ifeq ($(HOST_ARCH), aarch32)
CXX := $(XILINX_VITIS)/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-g++
else
$(error [ERROR]: Please check host architecture set.)
endif

XOCC := v++

include $(FPGA_PATH)/libgraph/xcl/xcl.mk
##################################################
#												 #
#		        Host source files				 #
#												 #
##################################################
# app 
HOST_SRCS = $(FPGA_APP_PATH)/main.cpp
HOST_SRCS += $(FPGA_APP_PATH)/dataPrepare.cpp
# kernel 
HOST_SRCS += $(FPGA_PATH)/libgraph/kernel/host_graph_kernel.cpp
# memory
HOST_SRCS += $(FPGA_PATH)/libgraph/memory/he_mapping.cpp
HOST_SRCS += $(FPGA_PATH)/libgraph/memory/he_mem.cpp
# misc
HOST_SRCS += $(FPGA_PATH)/libgraph/misc/data_helper.cpp
HOST_SRCS += $(FPGA_PATH)/libgraph/misc/graph.cpp
HOST_SRCS += $(FPGA_PATH)/libgraph/misc/host_graph_mem.cpp
# scheduler
HOST_SRCS += $(FPGA_PATH)/libgraph/scheduler/host_graph_scheduler.cpp
HOST_SRCS += $(FPGA_PATH)/libgraph/$(SCHEDULER)/scheduler.cpp
# libgraph
HOST_SRCS += $(FPGA_PATH)/libgraph/host_graph_dataflow.cpp
HOST_SRCS += $(FPGA_PATH)/libgraph/host_graph_partition.cpp
# xcl
HOST_SRCS += $(xcl_SRCS)
##################################################
#												 #
#		   Host compiler global settings	     #
#												 #
##################################################
# auto gen
AUTOGEN_CFLAG=-DSUB_PARTITION_NUM=$(SUB_PARTITION_NUM)
AUTOGEN_CFLAG+=-DPARTITION_SIZE=$(PARTITION_SIZE)
AUTOGEN_CFLAG+=-DDEFAULT_DATASET_PATH=\"$(DEFAULT_DATASET_PATH)\"
AUTOGEN_CFLAG+=-DXCL_BIN=\"$(XCLBIN)/graphitron.$(TARGET).$(DEVICE).xclbin\"
# define parameters
CXXFLAGS := $(AUTOGEN_CFLAG)
# opencl
include $(ABS_COMMON_REPO)/utils/opencl.mk
CXXFLAGS += $(opencl_CXXFLAGS) -Wall
# include
CXXFLAGS += -I/$(XILINX_SDX)/Vivado_HLS/include/ -O3 -g -fmessage-length=0 -std=c++14 -Wno-deprecated-declarations
CXXFLAGS += -I ./
CXXFLAGS += -I ./libfpga
CXXFLAGS += -I ./libgraph
CXXFLAGS += -I ./kernel
CXXFLAGS += -I $(FPGA_APP_PATH)
CXXFLAGS += -I ./libgraph/kernel
CXXFLAGS += -I ./libgraph/memory
CXXFLAGS += -I ./libgraph/misc
CXXFLAGS += -I ./libgraph/scheduler
CXXFLAGS += $(xcl_CXXFLAGS)
##################################################
#												 #
#		         Host linker flags		         #
#												 #
##################################################
LDFLAGS := $(opencl_LDFLAGS)
LDFLAGS += -lrt -lstdc++ -lxilinxopencl
LDFLAGS += $(xcl_CXXFLAGS)
##################################################
#												 #
#		  Kernel compiler global settings	     #
#												 #
##################################################
CLFLAGS := $(AUTOGEN_CFLAG)
# if Target == hw_emu
ifeq ($(TARGET), $(filter $(TARGET), hw_emu))
CLFLAGS += -g -t $(TARGET)
else
CLFLAGS += -t $(TARGET)
endif
CLFLAGS += --platform $(DEVICE) --save-temps -O3
CLFLAGS += -I ./
CLFLAGS += -I ./libfpga
CLFLAGS += -I $(FPGA_APP_PATH)
CLFLAGS += --xp prop:solution.kernel_compiler_margin=10%
##################################################
#												 #
#		        Kernel linker flags		         #
#												 #
##################################################
LDCLFLGAS += --xp prop:solution.kernel_compiler_margin=10% --kernel_frequency=$(FREQ)
##################################################
#												 #
#		   Specific Build Configuration	         #
#												 #
##################################################
VAR_TRUE = true

ifeq ($(strip $(HAVE_APPLY)), $(strip $(VAR_TRUE)))
CXXFLAGS += -DHAVE_APPLY=1
else
CXXFLAGS += -DHAVE_APPLY=0
endif

ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
CXXFLAGS += -DHAVE_EDGE_PROP=1
CLFLAGS  += -DHAVE_EDGE_PROP=1
else
CXXFLAGS += -DHAVE_EDGE_PROP=0
CLFLAGS  += -DHAVE_EDGE_PROP=0
endif

ifeq ($(strip $(HAVE_UNSIGNED_PROP)), $(strip $(VAR_TRUE)))
CXXFLAGS += -DHAVE_UNSIGNED_PROP=1
CLFLAGS  += -DHAVE_UNSIGNED_PROP=1
else
CXXFLAGS += -DHAVE_UNSIGNED_PROP=0
CLFLAGS  += -DHAVE_UNSIGNED_PROP=0
endif

ifeq ($(strip $(HAVE_APPLY_OUTDEG)), $(strip $(VAR_TRUE)))
CXXFLAGS += -DHAVE_APPLY_OUTDEG=1
CLFLAGS  += -DHAVE_APPLY_OUTDEG=1
else
CXXFLAGS += -DHAVE_APPLY_OUTDEG=0
CLFLAGS  += -DHAVE_APPLY_OUTDEG=0
endif

ifeq ($(strip $(CUSTOMIZE_APPLY)), $(strip $(VAR_TRUE)))
CXXFLAGS += -DCUSTOMIZE_APPLY=1
CLFLAGS  += -DCUSTOMIZE_APPLY=1
else
CXXFLAGS += -DCUSTOMIZE_APPLY=0
CLFLAGS  += -DCUSTOMIZE_APPLY=0
endif
## for hardware
CLFLAGS  += -DQUEUE_SIZE_FILTER=$(QUEUE_SIZE_FILTER)

CLFLAGS  += -DQUEUE_SIZE_MEMORY=$(QUEUE_SIZE_MEMORY)

CLFLAGS  += -DLOG_SCATTER_CACHE_BURST_SIZE=$(LOG_SCATTER_CACHE_BURST_SIZE)

CLFLAGS  += -DAPPLY_REF_ARRAY_SIZE=$(APPLY_REF_ARRAY_SIZE)
CXXFLAGS  += -DAPPLY_REF_ARRAY_SIZE=$(APPLY_REF_ARRAY_SIZE)

ifdef TARGET_PARTITION_SIZE
CLFLAGS  += -DTARGET_PARTITION_SIZE=$(TARGET_PARTITION_SIZE)
CXXFLAGS  += -DTARGET_PARTITION_SIZE=$(TARGET_PARTITION_SIZE)
endif