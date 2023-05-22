############# apply #############
$(XCLBIN)/alpha_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/alpha_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k alpha_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/alpha_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS += --nk alpha_kernel:1
BINARY_LINK_OBJS += --sp alpha_kernel_1.partSrcArray:HBM[0]
BINARY_LINK_OBJS += --sp alpha_kernel_1.partDstArray:HBM[1]
BINARY_LINK_OBJS += --sp alpha_kernel_1.partOutdegArray:HBM[2]

# insert

BINARY_LINK_OBJS += --slr alpha_kernel_1:SLR0
