compile: cleanbuild
	-$(MK) $(FPGA_APP_PATH)
	-$(RMDIR) $(FPGA_APP_PATH)/../tmp/
	-$(MK) $(FPGA_APP_PATH)/../tmp/
	$(CP) $(ABS_ROOT_REPO)/lib/$(OPT)/template/app/* $(FPGA_APP_PATH)
	$(CP) $(ABS_ROOT_REPO)/lib/$(OPT)/template/kernel $(FPGA_PATH)
	$(CP) $(ABS_ROOT_REPO)/lib/$(OPT)/libfpga $(FPGA_PATH)
	$(CP) $(ABS_ROOT_REPO)/lib/$(OPT)/libgraph $(FPGA_PATH)
	python3 ./graphitron_compiler/graphitronc.py -f ${APP_INPUT} -o ${FPGA_APP_PATH} -p $(OPT)
	-$(RMDIR) $(FPGA_APP_PATH)/../tmp/