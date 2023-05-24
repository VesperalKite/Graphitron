############# ep_kernel #############
$(XCLBIN)/ep_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/ep_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k ep_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/ep_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS += --nk ep_kernel:1
BINARY_LINK_OBJS += --sp ep_kernel_1.partSrcArray:HBM[0]
BINARY_LINK_OBJS += --sp ep_kernel_1.partDstArray:HBM[1]
# insert

BINARY_LINK_OBJS += --slr ep_kernel_1:SLR0
