ECHO:= @echo
.PHONY: help

help:
	${ECHO} "Make sure you have configure 'Graphitron.mk'"
	$(ECHO) ""
	${ECHO} "Makefile Usage:"
	$(ECHO) "  make app=<xxx> all"
	$(ECHO) "      Command to generate the design for specified Target and Device."
	$(ECHO) ""
	$(ECHO) "  make app=<xxx> cleanexe "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make app=<xxx> clean"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""