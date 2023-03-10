COMMON_REPO     = ./
ABS_COMMON_REPO = $(shell readlink -f $(COMMON_REPO))
UTILS_PATH      = ./utils
ROOT_REPO = ../../
ABS_ROOT_REPO = $(shell readlink -f $(ROOT_REPO))

include ${UTILS_PATH}/help.mk

.PHONY: all clean build

all: precheck
clean: precheck

precheck:
ifndef app
	$(error app is undefined)
else
APP = $(app)
APP_INPUT = ./apps/$(APP).gn

include ${UTILS_PATH}/utils.mk

include $(UTILS_PATH)/common.mk

include ${UTILS_PATH}/compile.mk
include ${UTILS_PATH}/clean.mk

all: build

endif