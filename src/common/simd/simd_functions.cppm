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

export module simd_functions;
import stl;
import simd_init;

namespace infinity {

export struct SIMD_FUNCTIONS {
    // F32 distance functions
    F32DistanceFuncType L2Distance_func_ptr_ = GetL2DistanceFuncPtr();
    F32DistanceFuncType IPDistance_func_ptr_ = GetIPDistanceFuncPtr();
    F32DistanceFuncType CosineDistance_func_ptr_ = GetCosineDistanceFuncPtr();

    // HNSW F32
    F32DistanceFuncType HNSW_F32L2_ptr_ = Get_HNSW_F32L2_ptr();
    F32DistanceFuncType HNSW_F32L2_16_ptr_ = Get_HNSW_F32L2_16_ptr();
    F32DistanceFuncType HNSW_F32IP_ptr_ = Get_HNSW_F32IP_ptr();
    F32DistanceFuncType HNSW_F32IP_16_ptr_ = Get_HNSW_F32IP_16_ptr();
    F32DistanceFuncType HNSW_F32Cos_ptr_ = Get_HNSW_F32Cos_ptr();
    F32DistanceFuncType HNSW_F32Cos_16_ptr_ = Get_HNSW_F32Cos_16_ptr();

    // HNSW I8
    I8DistanceFuncType HNSW_I8IP_ptr_ = Get_HNSW_I8IP_ptr();
    I8DistanceFuncType HNSW_I8IP_16_ptr_ = Get_HNSW_I8IP_16_ptr();
    I8DistanceFuncType HNSW_I8IP_32_ptr_ = Get_HNSW_I8IP_32_ptr();
    I8DistanceFuncType HNSW_I8IP_64_ptr_ = Get_HNSW_I8IP_64_ptr();

    // EMVB
    FilterScoresOutputIdsFuncType FilterScoresOutputIds_func_ptr_ = GetFilterScoresOutputIdsFuncPtr();

    // K-means
    SearchTop1WithDisF32U32FuncType SearchTop1WithDisF32U32_func_ptr_ = GetSearchTop1WithDisF32U32FuncPtr();
};

export const SIMD_FUNCTIONS &GetSIMD_FUNCTIONS() {
    static SIMD_FUNCTIONS simd_functions;
    return simd_functions;
}

} // namespace infinity
