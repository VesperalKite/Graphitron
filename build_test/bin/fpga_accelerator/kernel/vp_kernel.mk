############# vp_update_kernel #############
$(XCLBIN)/vp_update_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/vp_update_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k vp_update_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/vp_update_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS += --nk vp_update_kernel:1
BINARY_LINK_OBJS += --sp vp_update_kernel_1.partOutdegArray:HBM[2]
BINARY_LINK_OBJS += --sp vp_update_kernel_1.partRPA:HBM[3]
BINARY_LINK_OBJS += --sp vp_update_kernel_1.partCIA:HBM[4]
ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
BINARY_LINK_OBJS += --sp vp_update_kernel_1.partEdgeProp:HBM[5]
endif
BINARY_LINK_OBJS    += --sp  vp_update_kernel_1.parent:HBM[6]
BINARY_LINK_OBJS    += --sp  vp_update_kernel_1.parent_tmp:HBM[7]
BINARY_LINK_OBJS    += --sp  vp_update_kernel_1.prop_test:HBM[8]
BINARY_LINK_OBJS    += --sp  vp_update_kernel_1.activeVertex:HBM[9]
BINARY_LINK_OBJS    += --sp  vp_update_kernel_1.secondprop:HBM[10]
# insert

BINARY_LINK_OBJS += --slr vp_update_kernel_1:SLR0
############# vp_propapply_kernel #############
$(XCLBIN)/vp_propapply_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/vp_propapply_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k vp_propapply_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/vp_propapply_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS += --nk vp_propapply_kernel:1
BINARY_LINK_OBJS += --sp vp_propapply_kernel_1.partOutdegArray:HBM[2]
BINARY_LINK_OBJS += --sp vp_propapply_kernel_1.partRPA:HBM[3]
BINARY_LINK_OBJS += --sp vp_propapply_kernel_1.partCIA:HBM[4]
ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
BINARY_LINK_OBJS += --sp vp_propapply_kernel_1.partEdgeProp:HBM[5]
endif
BINARY_LINK_OBJS    += --sp  vp_propapply_kernel_1.parent:HBM[6]
BINARY_LINK_OBJS    += --sp  vp_propapply_kernel_1.parent_tmp:HBM[7]
BINARY_LINK_OBJS    += --sp  vp_propapply_kernel_1.prop_test:HBM[8]
BINARY_LINK_OBJS    += --sp  vp_propapply_kernel_1.activeVertex:HBM[9]
BINARY_LINK_OBJS    += --sp  vp_propapply_kernel_1.secondprop:HBM[10]
# insert

BINARY_LINK_OBJS += --slr vp_propapply_kernel_1:SLR0
