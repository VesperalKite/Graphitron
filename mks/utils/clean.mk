cleanexe:
	-$(RMDIR) $(EXECUTABLE)

clean:
	-$(RMDIR) $(FPGA_PATH)/*
	-$(RMDIR $(EXECUTABLE))