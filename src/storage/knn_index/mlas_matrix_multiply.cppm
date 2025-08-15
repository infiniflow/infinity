// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

export module infinity_core:mlas_matrix_multiply;

import std.compat;

namespace infinity {

export void
matrixA_multiply_matrixB_output_to_C(const float *x, const float *y, size_t x_row_num, size_t y_col_num, size_t common_dimension, float *output);

export void transpose_matrixA_multiply_matrixB_output_to_C(const float *x,
                                                           const float *y,
                                                           size_t x_col_num,
                                                           size_t y_col_num,
                                                           size_t common_dimension,
                                                           float *output);

export void matrixA_multiply_transpose_matrixB_output_to_C(const float *x, const float *y, size_t nx, size_t ny, size_t dimension, float *output);

export void transpose_matrixA_multiply_transpose_matrixB_output_to_C(const float *x,
                                                                     const float *y,
                                                                     size_t x_col_num,
                                                                     size_t y_row_num,
                                                                     size_t common_dimension,
                                                                     float *output);

export void TransposeMatrix(const float *input, float *output, size_t input_row_count, size_t input_column_count);

} // namespace infinity
