FPGA_PATH = ./fpga_accelerator
FPGA_APP_PATH = $(FPGA_PATH)/$(APP)
FPGA_KERNEL_PATH = $(FPGA_PATH)/kernel

build: cleanexe
	$(MK) $(FPGA_APP_PATH)
	$(CP) $(ROOT_REPO)/libfpga $(FPGA_PATH)
	$(CP) $(ROOT_REPO)/libgraph $(FPGA_PATH)
	python3 ./graphitron_compiler/graphitronc.py -f ${APP_INPUT} -o ${FPGA_APP_PATH}