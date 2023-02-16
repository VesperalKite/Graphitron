cleanexe:
	-$(RMDIR) $(EXECUTABLE)
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) sdaccel_* TempConfig system_estimate.xtxt *.rpt
	-$(RMDIR) src/*.ll _xocc_* .Xil dltmp* xmltmp* *.log *.jou *.wcfg *.wdb
	-$(RMDIR) .Xil
	-$(RMDIR) *.zip
	-$(RMDIR) *.str
	-$(RMDIR) $(XCLBIN)
	-$(RMDIR) ./_x
	-$(RMDIR) ./membership.out
	#-$(RMDIR) host_graph_fpga*
	-$(RMDIR) xclbin*
	-$(RMDIR) .run
	#-$(RMDIR) tmp_fpga_top
	#-$(RMDIR) tmp_para
	-$(RMDIR) para_gen
	-$(RMDIR) code_gen
	-$(RMDIR) makefile_gen
