//
// Created by yzq on 11/8/23.
//
module;
#include "/home/yzq/GitHub/infinity/third_party/mlas/inc/mlas.h"
#ifndef FINTEGER
#define FINTEGER long
#endif

export module use_mlas;

namespace infinity {
export void matrixA_multiply_transpose_matrixB_output_to_C(const float *x, const float *y, size_t nx, size_t ny, size_t dimension, float *output) {
    MlasGemm(CblasNoTrans, CblasTrans, nx, ny, dimension, 1.0f, x, dimension, y, dimension, 0.0f, output, ny, nullptr);
}
} // namespace infinity
