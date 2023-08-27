# Makefile for use with GNU make

THIS_MAKEFILE_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
THIS_DIR:=$(shell basename "${THIS_MAKEFILE_DIR}")
THIS_MAKEFILE:=$(lastword $(MAKEFILE_LIST))

CONFIGFILE ?= config.mk
include ${CONFIGFILE}

CONFIGFILEPATH=$(shell ls ${CONFIGFILE} >/dev/null 2>/dev/null && realpath ${CONFIGFILE})
ifeq (${CONFIGFILEPATH},)
  $(error Config file ${CONFIGFILE} not found)
endif

help:
	@echo "**** Welcome to ZSV/lib (alpha) ****"
	@echo
	@echo "This package has two primary components:"
	@echo "* zsvlib  : a fast CSV parser library"
	@echo "* zsv     : CSV editor and toolkit (that uses zsvlib)"
	@echo
	@echo "\`zsv\` also supports dynamic extensions, a sample of which you can"
	@echo "build and run as described in docs/extension.md"
	@echo
	@echo "To build, test and install zsvlib and zsv:"
	@echo "  ./configure && ${MAKE} test"
	@echo
	@echo "To build and install zsvlib and zsv:"
	@echo "  ./configure && ${MAKE} install"
	@echo
	@echo "To build and install only zsvlib:"
	@echo "  ./configure && ${MAKE} lib"
	@echo
	@echo "To build and install only zsv (i.e. install both, remove zsvlib):"
	@echo "  ./configure && ${MAKE} install && ${MAKE} uninstall-lib"
	@echo
	@echo "To save and build from a configuration without losing the current one,"
	@echo "use the configuration option CONFIGFILE e.g.:"
	@echo "  ./configure --config-file=/path/to/config.custom"
	@echo "  ./configure && ${MAKE} -C src CONFIGFILE=/path/to/config.custom install"
	@echo
	@echo "To clean (remove temporary build objects) (after running configure):"
	@echo "  ${MAKE} clean"
	@echo
	@echo "To uninstall libs and apps:"
	@echo "  ${MAKE} uninstall"
	@echo
	@echo "Additional make options available for the library or the apps by"
	@echo "  running ${MAKE} from the src or app directory"
	@echo
	@echo "For more information, see README.md"

lib:
	@${MAKE} -C src install CONFIGFILE=${CONFIGFILEPATH}

check test:
	@${MAKE} -C app test CONFIGFILE=${CONFIGFILEPATH}
	@${MAKE} -C examples/lib test CONFIGFILE=${CONFIGFILEPATH}

install:
	@${MAKE} -C src install CONFIGFILE=${CONFIGFILEPATH}
	@${MAKE} -C app install CONFIGFILE=${CONFIGFILEPATH}

all:
	@${MAKE} -C src install CONFIGFILE=${CONFIGFILEPATH}
	@${MAKE} -C app all CONFIGFILE=${CONFIGFILEPATH}

clean:
	@${MAKE} -C app clean-all CONFIGFILE=${CONFIGFILEPATH}
	@${MAKE} -C src clean CONFIGFILE=${CONFIGFILEPATH}
	@rm -rf ${THIS_MAKEFILE_DIR}/build

uninstall: uninstall-lib uninstall-app

uninstall-app:
	${MAKE} -C app uninstall CONFIGFILE=${CONFIGFILEPATH}

uninstall-lib:
	${MAKE} -C src uninstall CONFIGFILE=${CONFIGFILEPATH}

.PHONY: help install uninstall uninstall-app uninstall-lib check
