############# ep_compute_kernel #############
$(XCLBIN)/ep_compute_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/ep_compute_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k ep_compute_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/ep_compute_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS += --nk ep_compute_kernel:1
BINARY_LINK_OBJS += --sp ep_compute_kernel_1.partSrcArray:HBM[0]
BINARY_LINK_OBJS += --sp ep_compute_kernel_1.partDstArray:HBM[1]
BINARY_LINK_OBJS    += --sp  ep_compute_kernel_1.parent:HBM[6]
BINARY_LINK_OBJS    += --sp  ep_compute_kernel_1.parent_tmp:HBM[7]
BINARY_LINK_OBJS    += --sp  ep_compute_kernel_1.prop_test:HBM[8]
BINARY_LINK_OBJS    += --sp  ep_compute_kernel_1.activeVertex:HBM[9]
BINARY_LINK_OBJS    += --sp  ep_compute_kernel_1.secondprop:HBM[10]
# insert

BINARY_LINK_OBJS += --slr ep_compute_kernel_1:SLR0
