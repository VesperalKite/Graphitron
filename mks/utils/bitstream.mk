BINARY_CONTAINERS += $(XCLBIN)/graphitron.$(TARGET).$(DEVICE).xclbin
EXECUTABLE = host_graphitron_$(APP)

$(XCLBIN)/graphitron.$(TARGET).$(DEVICE).xclbin: $(BINARY_CONTAINER_OBJS)
	$(XOCC) $(CLFLAGS) -l $(LDCLFLGAS) $(BINARY_LINK_OBJS) -o '$@' $(+)

$(EXECUTABLE): $(HOST_SRCS) cleanexe
	mkdir -p $(XCLBIN)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) -o '$@' $(LDFLAGS)