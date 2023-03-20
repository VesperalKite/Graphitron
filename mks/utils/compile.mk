BINARY_CONTAINERS += $(XCLBIN)/graphitron.$(TARGET).$(DEVICE).xclbin
EXECUTABLE = host_graphitron_$(APP)


build: cleanexe
	$(MK) $(FPGA_APP_PATH)
	$(CP) $(ROOT_REPO)/lib/template/app/* $(FPGA_APP_PATH)
	$(CP) $(ROOT_REPO)/lib/template/kernel $(FPGA_PATH)
	$(CP) $(ROOT_REPO)/lib/libfpga $(FPGA_PATH)
	$(CP) $(ROOT_REPO)/lib/libgraph $(FPGA_PATH)
	python3 ./graphitron_compiler/graphitronc.py -f ${APP_INPUT} -o ${FPGA_APP_PATH}

$(XCLBIN)/graphitron.$(TARGET).$(DEVICE).xclbin: $(BINARY_CONTAINER_OBJS)
	$(XOCC) $(CLFLAGS) -l $(LDCLFLGAS) $(BINARY_LINK_OBJS) -o '$@' $(+)

$(EXECUTABLE): $(HOST_SRCS) cleanexe
	mkdir -p $(XCLBIN)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) -o '$@' $(LDFLAGS)
