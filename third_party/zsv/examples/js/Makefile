# Makefile for use with GNU make

THIS_MAKEFILE_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
THIS_MAKEFILE:=$(lastword $(MAKEFILE_LIST))

CC ?= emcc
SED ?= sed
NODE_PKG_NAME=zsv-lib

CONFIGFILE= ${THIS_MAKEFILE_DIR}/../../config.emcc
$(info Using config file ${CONFIGFILE})
include ${CONFIGFILE}
CONFIGFILEPATH=$(shell ls ${CONFIGFILE} >/dev/null 2>/dev/null && realpath ${CONFIGFILE})
ifeq ($(CONFIGFILEPATH),)
  $(error Config file ${CONFIGFILE} not found)
endif

ifeq ($(findstring emcc,$(CC)),) # emcc
  $(error Please use emcc to compile)
endif

ZSVSRC1=zsv.c ../app/utils/string.c
ZSVSRCDIR=${THIS_MAKEFILE_DIR}/../../src
ZSVSRC=$(addprefix ${ZSVSRCDIR}/,${ZSVSRC1})

INCLUDE_DIR=${THIS_MAKEFILE_DIR}/../../include

CFLAGS+= -I../../app/external/utf8proc-2.6.1 # for app/utils/string.c

ifeq ($(DEBUG),1)
  DBG_SUBDIR+=dbg
else
  DBG_SUBDIR+=rel
endif

CCBN=$(shell basename ${CC})
BUILD_DIR=${THIS_MAKEFILE_DIR}/build/${DBG_SUBDIR}/${CCBN}

BROWSER_JS=${BUILD_DIR}/zsv.js
INDEX=${BUILD_DIR}/index.html
EMJS=${BUILD_DIR}/zsv.em.js
WASM=${BUILD_DIR}/zsv.em.wasm

CFLAGS+= ${CFLAGS_PIC} -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS="['setValue','addFunction','removeFunction','writeArrayToMemory']" -s RESERVED_FUNCTION_POINTERS=4 -s EXPORTED_FUNCTIONS="['_free','_malloc']" -sASSERTIONS

ifeq ($(DEBUG),1)
  CFLAGS += ${CFLAGS_DEBUG}
else
  CFLAGS+= -O3 -DNDEBUG #  -std=gnu11 -Wno-gnu-statement-expression -Wshadow -Wall -Wextra -Wno-missing-braces -pedantic -DSTDC_HEADERS -D_GNU_SOURCE -lm -mavx2 -ftree-vectorize -flto
endif

BROWSER_STATIC=${BUILD_DIR}/index.html ${BUILD_DIR}/zsv-browser-example.js
OTHERUTIL=${BUILD_DIR}/localhost.pem
STATIC=${BROWSER_STATIC} ${OTHERUTIL}

##### test-related definitions
COLOR_NONE=\033[0m
COLOR_GREEN=\033[1;32m
COLOR_RED=\033[1;31m
COLOR_BLUE=\033[1;34m
COLOR_PINK=\033[1;35m

TEST_PASS=echo "${COLOR_BLUE}$@: ${COLOR_GREEN}Passed${COLOR_NONE}"
TEST_FAIL=(echo "${COLOR_BLUE}$@: ${COLOR_RED}Failed!${COLOR_NONE}" && exit 1)
#####

.PHONY: help all run clean prep node setup benchmark count_compare select_compare node_ok


help:
	@echo "make [build|run|node|test|clean] [NODE=/path/to/node]" # e.g. NODE=/usr/local/Cellar/node/19.1.0/bin/node
	@echo "by default, minified code is generated, which requires running the below once:"
	@echo "  make setup"
	@echo "alternatively, to generate non-minified code, use NO_MINIFY=1:"
	@echo "  make NO_MINIFY=1 [build|run|node|test|benchmark]"

build: ${BROWSER_JS} ${STATIC}
	@echo Built ${BROWSER_JS}

run: ${BROWSER_JS} ${STATIC}
	cd ${BUILD_DIR} && ${THIS_MAKEFILE_DIR}/browser/util/http-dev.py

clean:
	rm -rf build node

prep: ../../app/external/utf8proc-2.6.1/utf8proc.h

../../app/external/utf8proc-2.6.1/utf8proc.h:
	make -C ../../app ./external/utf8proc-2.6.1/utf8proc.h THIS_MAKEFILE_DIR=.

test: node_ok npm/test/select_all.js node
	@mkdir -p build/test
	@cp -p npm/test/select_all.js node/
	@echo "Running test (example) program \`node node/select_all.js ../../data/test/desc.csv\`"
	@(cd node && ${NODE} select_all.js ../../../data/test/desc.csv > ../build/test/out.json 2> ../build/test/out.err1)
	@sed 's/[0-9.]*ms//g' < build/test/out.err1 > build/test/out.err
	@cmp build/test/out.err npm/test/out.err
	@cmp build/test/out.json npm/test/out.json && ${TEST_PASS} || ${TEST_FAIL}

${BROWSER_STATIC}: ${BUILD_DIR}/% : browser/%
	@mkdir -p `dirname "$@"`
	@cp -p $< $@

${OTHERUTIL}: ${BUILD_DIR}/% : browser/util/%
	@mkdir -p `dirname "$@"`
	@cp -p $< $@

${EMJS}: ${ZSVSRC} zsv_parser_api_dummy.c
	@mkdir -p `dirname "$@"`
	${CC} ${CFLAGS} -I${INCLUDE_DIR} $^ -o $@

${WASM}: ${EMJS}

${BUILD_DIR}/zsv-browser-example.js:

${BROWSER_JS}: prep ${EMJS} js/head.js js/foot.js
	@mkdir -p `dirname "$@"`
	@cat js/head.js > $@.tmp.js
	@cat ${EMJS} >> $@.tmp.js
	@cat js/foot.js >> $@.tmp.js
ifeq ($(NO_MINIFY),1)
	@mv $@.tmp.js $@
else
	@uglifyjs $@.tmp.js -c -m > $@
	rm $@.tmp.js
endif

### node package build
NODE_INDEX=node/node_modules/${NODE_PKG_NAME}/index.js
NODE_WASM=node/node_modules/${NODE_PKG_NAME}/zsv.em.wasm
${NODE_WASM}: ${WASM}
	@mkdir -p `dirname "$@"`
	@cp -p $< $@

NODE_PKG_FILES=$(addprefix node/node_modules/${NODE_PKG_NAME}/, package.json README.md)
${NODE_PKG_FILES}: node/node_modules/${NODE_PKG_NAME}/% : npm/%
	@mkdir -p `dirname "$@"`
	@cp -p $< $@

${NODE_INDEX}: ${BROWSER_JS} js/foot_node.js
	@mkdir -p `dirname "$@"`
	@cat $^ > $@.tmp.js
ifeq ($(NO_MINIFY),1)
	@mv $@.tmp.js $@
else
	@uglifyjs $@.tmp.js -c -m > $@
	rm $@.tmp.js
endif

setup:
	npm install -g uglify-es

node: ${NODE_WASM} ${NODE_INDEX} ${NODE_PKG_FILES}
	@echo "Built node module in node/node_modules/${NODE_PKG_NAME}"


#### node benchmark
BENCHMARK_INPUT=${THIS_MAKEFILE_DIR}/../../app/benchmark/worldcitiespop_mil.csv

NODE=node --experimental-wasm-modules

NODE_VERSION=$(shell ${NODE} --version | sed 's/[.].*$$//' | sed 's/v//')
NODE_OK=$(shell [ "0${NODE_VERSION}" -gt "14" ] && echo "1" || echo "0")

node_ok:
ifneq ($(NODE_OK),1)
	echo "Node version must be at least 15"
	exit 1
endif

benchmark: node_ok node count_compare select_compare

count_compare: node_ok
	@cp -p npm/test/count*.js node/
	@cd node && (npm list | grep csv-parser) && echo "csv-parser already installed" || npm install csv-parser papaparse

	@echo "zsv count"
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/count.js 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/count.js 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/count.js 2>&1 | head -1

	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/count.js 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/count.js 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/count.js 2>&1 | head -1


	@echo "csv-parser count"
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/count-csv-parser.js 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/count-csv-parser.js 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/count-csv-parser.js 2>&1 | head -1

	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/count-csv-parser.js 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/count-csv-parser.js 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/count-csv-parser.js 2>&1 | head -1

	@echo "papaparse count"
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/count-papaparse.js 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/count-papaparse.js 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/count-papaparse.js 2>&1 | head -1

	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/count-papaparse.js 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/count-papaparse.js 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/count-papaparse.js 2>&1 | head -1

select_compare: node_ok
	@cp -p npm/test/select_all*.js node/
	@cd node && (npm list | grep csv-parser) && echo "csv-parser already installed" || npm install csv-parser papaparse

	@echo "zsv select_all"
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all.js '' '[0,2]' 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all.js '' '[0,2]' 2>&1 | head -1

	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all.js 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all.js 2>&1 | head -1

	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all.js '' '[0,2]' 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all.js '' '[0,2]' 2>&1 | head -1

	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all.js 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all.js 2>&1 | head -1

	@echo "csv-parser select_all"
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-csv-parser.js '' '[0,2]' 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-csv-parser.js '' '[0,2]' 2>&1 | head -1

	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-csv-parser.js 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-csv-parser.js 2>&1 | head -1

	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-csv-parser.js '' '[0,2]' 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-csv-parser.js '' '[0,2]' 2>&1 | head -1

	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-csv-parser.js 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-csv-parser.js 2>&1 | head -1

	@echo "papaparse select_all"
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-papaparse.js '' '[0,2]' 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-papaparse.js '' '[0,2]' 2>&1 | head -1

	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-papaparse.js 2>&1 | head -1
	head -5000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-papaparse.js 2>&1 | head -1

	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-papaparse.js '' '[0,2]' 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-papaparse.js '' '[0,2]' 2>&1 | head -1

	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-papaparse.js 2>&1 | head -1
	head -500000 ${BENCHMARK_INPUT} | ${NODE} node/select_all-papaparse.js 2>&1 | head -1
