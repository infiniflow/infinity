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
#include "simd_common_intrin_include.h"
module simd_init;
import stl;
import distance_simd_functions;
import hnsw_simd_func;
import maxsim_simd_funcs;
import emvb_simd_funcs;
import search_top_1_sgemm;

namespace infinity {

F32DistanceFuncType GetL2DistanceFuncPtr() {
#ifdef __AVX2__
    if (IsAVX2Supported()) {
        return &L2Distance_avx2;
    }
#endif
#ifdef __SSE2__
    if (IsSSE2Supported()) {
        return &F32L2SSEResidual;
    }
#endif
    return &L2Distance_common;
}

F32DistanceFuncType GetIPDistanceFuncPtr() {
#ifdef __AVX2__
    if (IsAVX2Supported()) {
        return &IPDistance_avx2;
    }
#endif
#ifdef __SSE2__
    if (IsSSE2Supported()) {
        return &F32IPSSEResidual;
    }
#endif
    return &IPDistance_common;
}

F32DistanceFuncType GetCosineDistanceFuncPtr() {
#ifdef __AVX2__
    if (IsAVX2Supported()) {
        return &CosineDistance_avx2;
    }
#endif
#ifdef __SSE2__
    if (IsSSE2Supported()) {
        return &F32CosSSEResidual;
    }
#endif
    return &CosineDistance_common;
}

U8HammingDistanceFuncType GetHammingDistanceFuncPtr() {
#ifdef __AVX2__
    return &HammingDistance_avx2;
#endif
#ifdef __SSE2__
    return &HammingDistance_sse2;
#endif
    return &HammingDistance_common;
}

F32DistanceFuncType Get_HNSW_F32L2_16_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &F32L2AVX512;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &F32L2AVX;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &F32L2SSE;
    }
#endif
    return &F32L2BF;
}

F32DistanceFuncType Get_HNSW_F32L2_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &F32L2AVX512Residual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &F32L2AVXResidual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &F32L2SSEResidual;
    }
#endif
    return &F32L2BF;
}

F32DistanceFuncType Get_HNSW_F32IP_16_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &F32IPAVX512;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &F32IPAVX;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &F32IPSSE;
    }
#endif
    return &F32IPBF;
}

F32DistanceFuncType Get_HNSW_F32IP_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &F32IPAVX512Residual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &F32IPAVXResidual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &F32IPSSEResidual;
    }
#endif
    return &F32IPBF;
}

F32DistanceFuncType Get_HNSW_F32Cos_16_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &F32CosAVX512;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &F32CosAVX;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &F32CosSSE;
    }
#endif
    return &F32CosBF;
}

F32DistanceFuncType Get_HNSW_F32Cos_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &F32CosAVX512Residual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &F32CosAVXResidual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &F32CosSSEResidual;
    }
#endif
    return &F32CosBF;
}

I8DistanceFuncType Get_HNSW_I8IP_64_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &I8IPAVX512;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &I8IPAVX;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &I8IPSSE;
    }
#endif
    return &I8IPBF;
}

I8DistanceFuncType Get_HNSW_I8IP_32_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &I8IPAVX512Residual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &I8IPAVX;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &I8IPSSE;
    }
#endif
    return &I8IPBF;
}

I8DistanceFuncType Get_HNSW_I8IP_16_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &I8IPAVX512Residual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &I8IPAVXResidual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &I8IPSSE;
    }
#endif
    return &I8IPBF;
}

I8DistanceFuncType Get_HNSW_I8IP_ptr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &I8IPAVX512Residual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &I8IPAVXResidual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &I8IPSSEResidual;
    }
#endif
    return &I8IPBF;
}

I8DistanceFuncType Get_HNSW_I8L2_64_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &I8L2AVX512BW;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &I8L2AVX2;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &I8L2SSE2;
    }
#endif
    return &I8L2BF;
}

I8DistanceFuncType Get_HNSW_I8L2_32_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &I8L2AVX512BWResidual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &I8L2AVX2;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &I8L2SSE2;
    }
#endif
    return &I8L2BF;
}

I8DistanceFuncType Get_HNSW_I8L2_16_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &I8L2AVX512BWResidual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &I8L2AVX2Residual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &I8L2SSE2;
    }
#endif
    return &I8L2BF;
}

I8DistanceFuncType Get_HNSW_I8L2_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &I8L2AVX512BWResidual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &I8L2AVX2Residual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &I8L2SSE2Residual;
    }
#endif
    return &I8L2BF;
}

I8CosDistanceFuncType Get_HNSW_I8Cos_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &I8CosAVX512BW;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &I8CosAVX2;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &I8CosSSE2;
    }
#endif
    return &I8CosBF;
}

U8DistanceFuncType Get_HNSW_U8L2_64_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &U8L2AVX512BW;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &U8L2AVX2;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &U8L2SSE2;
    }
#endif
    return &U8L2BF;
}

U8DistanceFuncType Get_HNSW_U8L2_32_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &U8L2AVX512BWResidual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &U8L2AVX2;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &U8L2SSE2;
    }
#endif
    return &U8L2BF;
}

U8DistanceFuncType Get_HNSW_U8L2_16_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &U8L2AVX512BWResidual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &U8L2AVX2Residual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &U8L2SSE2;
    }
#endif
    return &U8L2BF;
}

U8DistanceFuncType Get_HNSW_U8L2_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &U8L2AVX512BWResidual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &U8L2AVX2Residual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &U8L2SSE2Residual;
    }
#endif
    return &U8L2BF;
}

U8DistanceFuncType Get_HNSW_U8IP_64_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &U8IPAVX512BW;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &U8IPAVX2;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &U8IPSSE2;
    }
#endif
    return &U8IPBF;
}

U8DistanceFuncType Get_HNSW_U8IP_32_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &U8IPAVX512BWResidual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &U8IPAVX2;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &U8IPSSE2;
    }
#endif
    return &U8IPBF;
}

U8DistanceFuncType Get_HNSW_U8IP_16_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &U8IPAVX512BWResidual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &U8IPAVX2Residual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &U8IPSSE2;
    }
#endif
    return &U8IPBF;
}

U8DistanceFuncType Get_HNSW_U8IP_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &U8IPAVX512BWResidual;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &U8IPAVX2Residual;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &U8IPSSE2Residual;
    }
#endif
    return &U8IPBF;
}

U8CosDistanceFuncType Get_HNSW_U8Cos_ptr() {
#if defined(__AVX512BW__)
    if (IsAVX512BWSupported()) {
        return &U8CosAVX512BW;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &U8CosAVX2;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &U8CosSSE2;
    }
#endif
    return &U8CosBF;
}

MaxSimF32BitIPFuncType GetMaxSimF32BitIPFuncPtr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &maxsim_f32_bit_ip_avx512;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &maxsim_f32_bit_ip_avx2;
    }
#endif
    return &maxsim_f32_bit_ip_plain;
}

MaxSimI32BitIPFuncType GetMaxSimI32BitIPFuncPtr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &maxsim_i32_bit_ip_avx512;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &maxsim_i32_bit_ip_avx2;
    }
#endif
    return &maxsim_i32_bit_ip_plain;
}

MaxSimI64BitIPFuncType GetMaxSimI64BitIPFuncPtr() {
#if defined(__AVX512F__)
    if (IsAVX512Supported()) {
        return &maxsim_i64_bit_ip_avx512;
    }
#endif
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &maxsim_i64_bit_ip_avx2;
    }
#endif
    return &maxsim_i64_bit_ip_plain;
}

FilterScoresOutputIdsFuncType GetFilterScoresOutputIdsFuncPtr() {
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &filter_scores_output_ids_avx2;
    }
#endif
    return &filter_scores_output_ids_common;
}

SearchTop1WithDisF32U32FuncType GetSearchTop1WithDisF32U32FuncPtr() {
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &search_top_1_with_dis_avx2<u32>;
    }
#endif
#if defined(__SSE2__)
    if (IsSSE2Supported()) {
        return &search_top_1_with_dis_sse2<u32>;
    }
#endif
    return &search_top_1_simple_with_dis<f32, f32, u32, f32>;
}

} // namespace infinity
