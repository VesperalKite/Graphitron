compile: clean
	$(MK) $(FPGA_APP_PATH)
	$(CP) $(ABS_ROOT_REPO)/lib/template/app/* $(FPGA_APP_PATH)
	$(CP) $(ABS_ROOT_REPO)/lib/template/kernel $(FPGA_PATH)
	$(CP) $(ABS_ROOT_REPO)/lib/libfpga $(FPGA_PATH)
	$(CP) $(ABS_ROOT_REPO)/lib/libgraph $(FPGA_PATH)
	python3 ./graphitron_compiler/graphitronc.py -f ${APP_INPUT} -o ${FPGA_APP_PATH}