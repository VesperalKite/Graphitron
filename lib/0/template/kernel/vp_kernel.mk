############# vp_kernel #############
$(XCLBIN)/vp_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/vp_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k vp_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/vp_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS += --nk vp_kernel:1
BINARY_LINK_OBJS += --sp vp_kernel_1.partOutdegArray:HBM[2]
BINARY_LINK_OBJS += --sp vp_kernel_1.partRPA:HBM[3]
BINARY_LINK_OBJS += --sp vp_kernel_1.partCIA:HBM[4]
# insert

BINARY_LINK_OBJS += --slr vp_kernel_1:SLR0
