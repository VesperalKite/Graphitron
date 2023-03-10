
build: cleanexe
	$(MK) $(FPGA_APP_PATH)
	$(CP) $(ROOT_REPO)/lib/libfpga $(FPGA_PATH)
	$(CP) $(ROOT_REPO)/lib/libgraph $(FPGA_PATH)
	python3 ./graphitron_compiler/graphitronc.py -f ${APP_INPUT} -o ${FPGA_APP_PATH}