############# apply #############
ifeq ($(strip $(HAVE_APPLY)), $(strip $(VAR_TRUE)))
$(XCLBIN)/apply_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/apply_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k apply_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/apply_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS    += --nk  apply_kernel:1
BINARY_LINK_OBJS    += --sp  apply_kernel_1.vertexProp:HBM[2]
BINARY_LINK_OBJS    += --sp  apply_kernel_1.newVertexProp0:HBM[2]
BINARY_LINK_OBJS    += --sp  apply_kernel_1.tmpVertexProp0:HBM[3]
BINARY_LINK_OBJS    += --sp  apply_kernel_1.outDegree:HBM[5]

# insert

BINARY_LINK_OBJS    += --slr apply_kernel_1:SLR0
endif
