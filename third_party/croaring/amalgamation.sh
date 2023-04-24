#!/bin/bash
########################################################################
# Generates an "amalgamation build" for roaring. Inspired by similar
# script used by whefs.
########################################################################
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

timestamp=$(date -u +"%Y-%m-%dT%H:%M:%SZ")  # capture to label files with their generation time

function newline {
    echo ""
}

echo "We are about to amalgamate all CRoaring files into one source file."
echo "For rationale, see: https://www.sqlite.org/amalgamation.html"
echo "and: https://en.wikipedia.org/wiki/Single_Compilation_Unit"
newline


# Files that this script generates
#
AMAL_H="roaring.h"
AMAL_HH="roaring.hh"
AMAL_C="roaring.c"
DEMOC="amalgamation_demo.c"
DEMOCPP="amalgamation_demo.cpp"


# .h files for the official API => Order matters
#
ALL_PUBLIC_H="
$SCRIPTPATH/include/roaring/roaring_version.h
$SCRIPTPATH/include/roaring/roaring_types.h
$SCRIPTPATH/include/roaring/roaring.h
$SCRIPTPATH/include/roaring/memory.h
"

# .hh header files for the C++ API wrapper => Order does not matter at present
#
ALL_PUBLIC_HH="
$SCRIPTPATH/cpp/roaring.hh
$SCRIPTPATH/cpp/roaring64map.hh
"

# internal .h files => These are used in the implementation but aren't part of
# the API.  They are all embedded at the head of the amalgamated C file, and
# need to be in this order.
#
ALL_PRIVATE_H="
$SCRIPTPATH/include/roaring/portability.h
$SCRIPTPATH/include/roaring/isadetection.h
$SCRIPTPATH/include/roaring/containers/perfparameters.h
$SCRIPTPATH/include/roaring/containers/container_defs.h
$SCRIPTPATH/include/roaring/array_util.h
$SCRIPTPATH/include/roaring/utilasm.h
$SCRIPTPATH/include/roaring/bitset_util.h
$SCRIPTPATH/include/roaring/containers/array.h
$SCRIPTPATH/include/roaring/containers/bitset.h
$SCRIPTPATH/include/roaring/containers/run.h
$SCRIPTPATH/include/roaring/containers/convert.h
$SCRIPTPATH/include/roaring/containers/mixed_equal.h
$SCRIPTPATH/include/roaring/containers/mixed_subset.h
$SCRIPTPATH/include/roaring/containers/mixed_andnot.h
$SCRIPTPATH/include/roaring/containers/mixed_intersection.h
$SCRIPTPATH/include/roaring/containers/mixed_negation.h
$SCRIPTPATH/include/roaring/containers/mixed_union.h
$SCRIPTPATH/include/roaring/containers/mixed_xor.h
$SCRIPTPATH/include/roaring/containers/containers.h
$SCRIPTPATH/include/roaring/roaring_array.h
"

# .c implementation files
#
# The `#include <roaring/*>` lines are stripped out with sed, so all the C code
# has the definitions available from all the header files.  Since the order of
# the top level declarations doesn't matter after that point, the file list is
# generated automatically from git-tracked C files in the /src/ directory.
#
ALL_PRIVATE_C=$( ( \
    [ -d $SCRIPTPATH/.git ] \
        && ( type git >/dev/null 2>&1 ) \
        && ( git ls-files $SCRIPTPATH/src/*.c $SCRIPTPATH/src/**/*c ) \
    ) || ( find $SCRIPTPATH/src -name '*.c' ) )

# Verify up-front that all the files exist
#
for i in ${ALL_PUBLIC_H} ${ALL_PUBLIC_HH} ${ALL_PRIVATE_H} ${ALL_PRIVATE_C}; do
    test -e $i && continue
    echo "FATAL: source file [$i] not found."
    exit 127
done

function echo_timestamp()
{
    echo "// !!! DO NOT EDIT - THIS IS AN AUTO-GENERATED FILE !!!"
    echo "// Created by amalgamation.sh on ${timestamp}"
    newline
}

function echo_license()
{
    cat $SCRIPTPATH/src/license-comment.h
    newline
}

function stripinc()
{
    sed -e '/# *include *"/d' -e '/# *include *<roaring\//d'
}

function dofile()
{
    RELFILE=${1#"$SCRIPTPATH/"}
    echo "/* begin file $RELFILE */"

    # The preprocessor has a feature which lets you redefine the line and file:
    # https://en.cppreference.com/w/c/preprocessor/line
    # 
    # This conceivably could be used to map back to the original source, e.g.:
    #
    #     echo "#line 8 \"$1\""
    #
    # However, this breaks IDEs and is not nearly as useful as it sounds.

    stripinc < $1
    echo "/* end file $RELFILE */"
}

echo "Creating ${AMAL_H}..."
{
    echo_timestamp
    echo_license

    for h in ${ALL_PUBLIC_H}; do
        dofile $h
    done
} > "${AMAL_H}"


echo "Creating ${AMAL_C}..."
{
    echo_timestamp
    echo_license
  
    echo "#include \"${AMAL_H}\""

    newline
    echo "/* used for http://dmalloc.com/ Dmalloc - Debug Malloc Library */"
    echo "#ifdef DMALLOC"
    echo "#include \"dmalloc.h\""
    echo "#endif"
    newline

    echo "#include \"roaring.h\"  /* include public API definitions */"

    for h in ${ALL_PRIVATE_H} ${ALL_PRIVATE_C}; do
        dofile $h
    done
} > "${AMAL_C}"


echo "Creating ${DEMOC}..."
{
    echo_timestamp

    cat <<< '
#include <stdio.h>
#include "roaring.c"
int main() {
  roaring_bitmap_t *r1 = roaring_bitmap_create();
  for (uint32_t i = 100; i < 1000; i++) roaring_bitmap_add(r1, i);
  printf("cardinality = %d\n", (int) roaring_bitmap_get_cardinality(r1));
  roaring_bitmap_free(r1);
  return 0;
}
'
} > "${DEMOC}"


echo "Creating ${AMAL_HH}..."
{
    echo_timestamp
    echo_license

    # using the C++ roaring:: namespace does not make the C API available by
    # default.  One must either `#include "roaring.h"` manually or use the
    # `roaring::api::` namespace.  The protection against putting the API in
    # global scope is done via a #define surrounding the include, but that
    # inclusion is stripped out so we repeat it here.
    #
    # !!! This would be better if it found the inclusion of roaring.h and
    # replaced it with the amalgamated file.
    #
    echo "#define ROARING_API_NOT_IN_GLOBAL_NAMESPACE  // see remarks in roaring.h"
    echo "#include \"${AMAL_H}\""
    echo "#undef ROARING_API_NOT_IN_GLOBAL_NAMESPACE"

    for hh in ${ALL_PUBLIC_HH}; do
        dofile $hh
    done
} > "${AMAL_HH}"


echo "Creating ${DEMOCPP}..."
{
    echo_timestamp
    echo_license

    cat <<< '
#include <iostream>
#include "roaring.hh"
#include "roaring.c"
int main() {
  roaring::Roaring r1;
  for (uint32_t i = 100; i < 1000; i++) {
    r1.add(i);
  }
  std::cout << "cardinality = " << r1.cardinality() << std::endl;

  roaring::Roaring64Map r2;
  for (uint64_t i = 18000000000000000100ull; i < 18000000000000001000ull; i++) {
    r2.add(i);
  }
  std::cout << "cardinality = " << r2.cardinality() << std::endl;
  return 0;
}
'
} >  "${DEMOCPP}"


# Print out a directory listing of the output files and their sizes
#
newline
echo "Files have been written to current directory: $PWD "
ls -la ${AMAL_C} ${AMAL_H} ${AMAL_HH}  ${DEMOC} ${DEMOCPP}
newline

CBIN=${DEMOC%%.*}
CPPBIN=${DEMOCPP%%.*}

echo "The interface is found in the file 'include/roaring/roaring.h'."
newline
echo "For C, try:"
echo "cc -march=native -O3 -std=c11  -o ${CBIN} ${DEMOC}  && ./${CBIN} "
newline
echo "For C++, try:"
echo "c++ -march=native -O3 -std=c++11 -o ${CPPBIN} ${DEMOCPP}  && ./${CPPBIN} "

lowercase(){
    echo "$1" | tr 'A-Z' 'a-z'
}

OS=`lowercase \`uname\``

newline
echo "You can build a shared library with the following command:"

if [ $OS == "darwin" ]; then
  echo "cc -march=native -O3 -std=c11 -shared -o libroaring.dylib -fPIC roaring.c"
else
  echo "cc -march=native -O3 -std=c11 -shared -o libroaring.so -fPIC roaring.c"
fi

