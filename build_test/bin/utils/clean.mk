cleanexe:
	-$(RMDIR) $(EXECUTABLE)
cleanbuild:
	-$(RMDIR) $(FPGA_APP_PATH)
	-$(RMDIR) $(FPGA_PATH)/kernel
	-$(RMDIR) $(FPGA_PATH)/libfpga
	-$(RMDIR) $(FPGA_PATH)/libgraph
clean:
	-$(RMDIR) $(FPGA_PATH)/*
	-$(RMDIR) $(EXECUTABLE)