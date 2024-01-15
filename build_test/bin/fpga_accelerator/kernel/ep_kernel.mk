############# ep_computeContrib_kernel #############
$(XCLBIN)/ep_computeContrib_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/ep_computeContrib_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k ep_computeContrib_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/ep_computeContrib_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS += --nk ep_computeContrib_kernel:1
BINARY_LINK_OBJS += --sp ep_computeContrib_kernel_1.partSrcArray:HBM[0]
BINARY_LINK_OBJS += --sp ep_computeContrib_kernel_1.partDstArray:HBM[1]
BINARY_LINK_OBJS    += --sp  ep_computeContrib_kernel_1.old_rank:HBM[6]
BINARY_LINK_OBJS    += --sp  ep_computeContrib_kernel_1.new_rank:HBM[7]
BINARY_LINK_OBJS    += --sp  ep_computeContrib_kernel_1.contrib:HBM[8]
BINARY_LINK_OBJS    += --sp  ep_computeContrib_kernel_1.error:HBM[9]
# insert

BINARY_LINK_OBJS += --slr ep_computeContrib_kernel_1:SLR0
