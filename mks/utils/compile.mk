compile: cleanbuild
	-$(MK) $(FPGA_APP_PATH)
	-$(RMDIR) $(FPGA_APP_PATH)/../tmp/
	-$(MK) $(FPGA_APP_PATH)/../tmp/
ifeq ($(strip $(OPT)), 1)
	$(CP) $(ABS_ROOT_REPO)/lib/template/app/* $(FPGA_APP_PATH)
	$(CP) $(ABS_ROOT_REPO)/lib/template/kernel $(FPGA_PATH)
endif
	$(CP) $(ABS_ROOT_REPO)/lib/libfpga $(FPGA_PATH)
	$(CP) $(ABS_ROOT_REPO)/lib/libgraph $(FPGA_PATH)
	python3 ./graphitron_compiler/graphitronc.py -f ${APP_INPUT} -o ${FPGA_APP_PATH} -p $(OPT)
	-$(RMDIR) $(FPGA_APP_PATH)/../tmp/