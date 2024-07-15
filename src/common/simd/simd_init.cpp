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
#include "NGT/Common.h"

module simd_init;
import stl;
import distance_simd_functions;
import hnsw_simd_func;
import emvb_simd_funcs;

namespace infinity {

struct SupportedSimdTypes {
    Vector<char const *> types_strs_ = NGT::CpuInfo::getSupportedSimdTypes();
    bool is_sse2_ = NGT::CpuInfo::isSSE2();
    bool is_avx2_ = NGT::CpuInfo::isAVX2();
    bool is_avx512_ = NGT::CpuInfo::isAVX512();
};

const SupportedSimdTypes &GetSupportedSimdTypes() {
    static SupportedSimdTypes supported_simd_types;
    return supported_simd_types;
}

const Vector<char const *> &GetSupportedSimdTypesList() { return GetSupportedSimdTypes().types_strs_; }

bool IsSSE2Supported() { return GetSupportedSimdTypes().is_sse2_; }

bool IsAVX2Supported() { return GetSupportedSimdTypes().is_avx2_; }

bool IsAVX512Supported() { return GetSupportedSimdTypes().is_avx512_; }

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

FilterScoresOutputIdsFuncType GetFilterScoresOutputIdsFuncPtr() {
#if defined(__AVX2__)
    if (IsAVX2Supported()) {
        return &filter_scores_output_ids_avx2;
    }
#endif
    return &filter_scores_output_ids_common;
}

} // namespace infinity
