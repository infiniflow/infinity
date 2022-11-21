#!/usr/bin/env bash
set -ex

rm -Rvf CMakeCache.txt session.data CMakeFiles
ccache -C

CXX=clang++ cmake -G Ninja -DNB_cxx_standard=17 -DCMAKE_CXX_FLAGS="-O2 -ftime-trace" -DCMAKE_BUILD_TYPE=Release ../..
# CXX=clang++ cmake -G Ninja -DCMAKE_CXX_FLAGS="-ftime-trace" -DCMAKE_BUILD_TYPE=Debug ../..

~/git/ClangBuildAnalyzer/bin/ClangBuildAnalyzer --start .
time ninja
~/git/ClangBuildAnalyzer/bin/ClangBuildAnalyzer --stop . session.data
~/git/ClangBuildAnalyzer/bin/ClangBuildAnalyzer --analyze session.data
