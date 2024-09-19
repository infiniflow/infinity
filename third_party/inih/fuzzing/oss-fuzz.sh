#!/usr/bin/env bash
$CC $CFLAGS -c ../ini.c
$CC $CFLAGS -c inihfuzz.c
$CXX $CFLAGS $LIB_FUZZING_ENGINE inihfuzz.o ini.o -o inihfuzz
