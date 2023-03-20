############# gs #############
$(XCLBIN)/gs_kernel.$(TARGET).$(DEVICE).xo: $(FPGA_KERNEL_PATH)/gs_kernel.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k gs_kernel -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/gs_kernel.$(TARGET).$(DEVICE).xo
BINARY_LINK_OBJS    += --nk  gs_kernel:1
BINARY_LINK_OBJS    += --sp  gs_kernel_1.edgesHeadArray:HBM[0]
BINARY_LINK_OBJS    += --sp  gs_kernel_1.edgesTailArray:HBM[1]
BINARY_LINK_OBJS    += --sp  gs_kernel_1.vertexPushinProp:HBM[2]
BINARY_LINK_OBJS    += --sp  gs_kernel_1.tmpVertexProp:HBM[3]
ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
BINARY_LINK_OBJS    += --sp  gs_kernel_1.edgeProp:HBM[4]
endif
BINARY_LINK_OBJS    += --slr gs_kernel_1:SLR0
