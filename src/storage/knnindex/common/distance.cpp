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

module;

#include "faiss/impl/platform_macros.h"

import stl;

module distance;

namespace infinity {

FAISS_PRAGMA_IMPRECISE_FUNCTION_BEGIN
template <typename ElemType>
ElemType fvec_norm_L2sqr(const ElemType *query, SizeT dimension) {
    ElemType res = 0;
    FAISS_PRAGMA_IMPRECISE_LOOP
    for (size_t i = 0; i != dimension; ++i) {
        res += query[i] * query[i];
    }

    return res;
}
FAISS_PRAGMA_IMPRECISE_FUNCTION_END

FAISS_PRAGMA_IMPRECISE_FUNCTION_BEGIN
template <>
f32 fvec_norm_L2sqr(const f32 *query, SizeT dimension) {
    f32 res = 0;
    FAISS_PRAGMA_IMPRECISE_LOOP
    for (size_t i = 0; i != dimension; ++i) {
        res += query[i] * query[i];
    }

    return res;
}
FAISS_PRAGMA_IMPRECISE_FUNCTION_END

template <typename DistType>
void fvec_norms_L2sqr(DistType *__restrict nr, const DistType *__restrict queries, SizeT dimension, u16 query_count) {
    for (int64_t i = 0; i < query_count; i++) {
        nr[i] = fvec_norm_L2sqr(queries + i * dimension, dimension);
    }
}

template <>
void fvec_norms_L2sqr<f32>(f32 *__restrict nr, const f32 *__restrict queries, SizeT dimension, u16 query_count) {
    for (int64_t i = 0; i < query_count; i++) {
        nr[i] = fvec_norm_L2sqr<f32>(queries + i * dimension, dimension);
    }
}

} // namespace infinity
