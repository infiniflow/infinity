//
// Created by jinhai on 23-5-2.
//

#pragma once

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Following functions are copied from FAISS

float*
fvecs_read(const char* fname, size_t* d_out, size_t* n_out);

int*
ivecs_read(const char* fname, size_t* d_out, size_t* n_out);
