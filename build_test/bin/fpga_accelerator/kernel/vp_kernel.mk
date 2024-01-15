############# vp_computeError_kernel #############
$(XCLBIN)/vp_computeError_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/vp_computeError_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k vp_computeError_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/vp_computeError_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS += --nk vp_computeError_kernel:1
BINARY_LINK_OBJS += --sp vp_computeError_kernel_1.partOutdegArray:HBM[2]
BINARY_LINK_OBJS += --sp vp_computeError_kernel_1.partRPA:HBM[3]
BINARY_LINK_OBJS += --sp vp_computeError_kernel_1.partCIA:HBM[4]
ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
BINARY_LINK_OBJS += --sp vp_computeError_kernel_1.partEdgeProp:HBM[5]
endif
BINARY_LINK_OBJS    += --sp  vp_computeError_kernel_1.old_rank:HBM[6]
BINARY_LINK_OBJS    += --sp  vp_computeError_kernel_1.new_rank:HBM[7]
BINARY_LINK_OBJS    += --sp  vp_computeError_kernel_1.contrib:HBM[8]
BINARY_LINK_OBJS    += --sp  vp_computeError_kernel_1.error:HBM[9]
# insert

BINARY_LINK_OBJS += --slr vp_computeError_kernel_1:SLR0
############# vp_updateVertex_kernel #############
$(XCLBIN)/vp_updateVertex_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/vp_updateVertex_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k vp_updateVertex_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/vp_updateVertex_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS += --nk vp_updateVertex_kernel:1
BINARY_LINK_OBJS += --sp vp_updateVertex_kernel_1.partOutdegArray:HBM[2]
BINARY_LINK_OBJS += --sp vp_updateVertex_kernel_1.partRPA:HBM[3]
BINARY_LINK_OBJS += --sp vp_updateVertex_kernel_1.partCIA:HBM[4]
ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
BINARY_LINK_OBJS += --sp vp_updateVertex_kernel_1.partEdgeProp:HBM[5]
endif
BINARY_LINK_OBJS    += --sp  vp_updateVertex_kernel_1.old_rank:HBM[6]
BINARY_LINK_OBJS    += --sp  vp_updateVertex_kernel_1.new_rank:HBM[7]
BINARY_LINK_OBJS    += --sp  vp_updateVertex_kernel_1.contrib:HBM[8]
BINARY_LINK_OBJS    += --sp  vp_updateVertex_kernel_1.error:HBM[9]
# insert

BINARY_LINK_OBJS += --slr vp_updateVertex_kernel_1:SLR0
