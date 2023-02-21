COMMON_REPO     = ./
ABS_COMMON_REPO = $(shell readlink -f $(COMMON_REPO))
UTILS_PATH      = ./utils

include ${UTILS_PATH}/help.mk

.PHONY: all clean ${COMPILE_OUT}

all: precheck
clean: precheck

precheck:
ifndef app
	$(error app is undefined)
else
APP = ./apps/$(app).gn

include ${UTILS_PATH}/utils.mk

include ${UTILS_PATH}/compile.mk
include ${UTILS_PATH}/clean.mk

all: ${COMPILE_OUT}

endif