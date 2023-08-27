# Makefile for use with GNU make

THIS_MAKEFILE_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
THIS_DIR:=$(shell basename "${THIS_MAKEFILE_DIR}")
THIS_MAKEFILE:=$(lastword $(MAKEFILE_LIST))

CONFIGFILE ?= ../../config.mk
include ${CONFIGFILE}

CONFIGFILEPATH=$(shell ls ${CONFIGFILE} >/dev/null 2>/dev/null && realpath ${CONFIGFILE})
ifeq (${CONFIGFILEPATH},)
  $(error Config file ${CONFIGFILE} not found)
endif

ifeq ($(MAKE),)
  MAKE=make
endif

CFLAGS+= -I${PREFIX}/include
ifeq ($(ZSV_EXTRAS),1)
  CFLAGS+= -DZSV_EXTRAS
endif

WIN=
ifeq ($(WIN),)
  WIN=0
  ifneq ($(findstring w64,$(CC)),) # e.g. mingw64
    WIN=1
  endif
endif

TEST_DATA_DIR=${THIS_MAKEFILE_DIR}/../../data
TMP_DIR=/tmp
COLOR_NONE=\033[0m
COLOR_GREEN=\033[1;32m
COLOR_RED=\033[1;31m
COLOR_BLUE=\033[1;34m
COLOR_PINK=\033[1;35m

TEST_PASS=echo "${COLOR_BLUE}$@: ${COLOR_GREEN}Passed${COLOR_NONE}"
TEST_FAIL=(echo "${COLOR_BLUE}$@: ${COLOR_RED}Failed!${COLOR_NONE}" && exit 1)

EXE=
ifeq ($(WIN),1)
  EXE=.exe
endif

CFLAGS+=-g -O0

BUILD_DIR=build
LIBS+=-lzsv

help:
	@echo "**** Examples using libzsv ****"
	@echo
	@echo "Dependencies:"
	@echo "  - libzsv must already be installed (for more info, \`cd ../.. && ${MAKE}\`)"
	@echo "  - configuration file must be available (to generate, \`cd ../.. && ./configure\`)"
	@echo
	@echo "To build and test using the default configuration:"
	@echo "  ${MAKE} build test"
	@echo
	@echo "To build using a specified configuration:"
	@echo "  ${MAKE} CONFIGFILE=/path/to/config.mk build"
	@echo
	@echo "To build a specific example:"
	@echo "  ${MAKE} simple|print_my_column|parse_by_chunk|pull"
	@echo
	@echo "To remove all build files:"
	@echo "  ${MAKE} clean"
	@echo

build: simple print_my_column parse_by_chunk pull

test: test-eol test-tiny

test-tiny: build/simple${EXE}
	@[ "`echo '' | $< - 2>&1`" = "" ] && ${TEST_PASS} || ${TEST_FAIL}

test-eol: test-eol-1 test-eol-2 test-eol-3 test-eol-4

test-eol-%: build/simple${EXE} build/pull${EXE}
	@$< ${TEST_DATA_DIR}/test/no-eol-$*.csv > ${TMP_DIR}/$@.out
	@cmp ${TMP_DIR}/$@.out test/expected/$@.out && ${TEST_PASS} || ${TEST_FAIL}

	@build/pull${EXE} ${TEST_DATA_DIR}/test/no-eol-$*.csv > ${TMP_DIR}/$@.out
	@cmp ${TMP_DIR}/$@.out test/expected/$@.out && ${TEST_PASS} || ${TEST_FAIL}

simple print_my_column parse_by_chunk pull: % : ${BUILD_DIR}/%${EXE}
	@echo Built $<

${BUILD_DIR}/print_my_column${EXE} ${BUILD_DIR}/simple${EXE} ${BUILD_DIR}/parse_by_chunk${EXE} ${BUILD_DIR}/pull${EXE}: ${BUILD_DIR}/%${EXE} : %.c
	@mkdir -p `dirname "$@"`
	${CC} ${CFLAGS} -o $@ $< ${LIBS} -L${LIBDIR}

clean:
	@rm -rf ${BUILD_DIR}

.PHONY: help build clean simple print_my_column parse_by_chunk pull
