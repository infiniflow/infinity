//
// Copyright (C) 2015 Yahoo Japan Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// NOTOCE: This file includes codes modified from NGT v2.2.2, which is licensed under the Apache License, Version 2.0.
// NOTICE: For more information go to https://github.com/yahoojapan/NGT

#pragma once
#include <vector>

namespace NGT {
class CpuInfo {
public:
    enum SimdType {
        SimdTypeF16C,
        SimdTypeSSE2,
        SimdTypeAVX,
        SimdTypeAVX2,
        SimdTypeAVX512F,
        SimdTypeAVX512VL,
        SimdTypeAVX512BW,
        SimdTypeAVX512DQ,
        SimdTypeAVX512CD,
        SimdTypeAVX512ER,
        SimdTypeAVX512PF,
        SimdTypeAVX512VBMI,
        SimdTypeAVX512IFMA,
        SimdTypeAVX5124VNNIW,
        SimdTypeAVX5124FMAPS,
        SimdTypeAVX512VPOPCNTDQ,
        SimdTypeAVX512VBMI2,
        SimdTypeAVX512VNNI,
    };
    static bool is(SimdType type) {
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
        __builtin_cpu_init();
        switch (type) {
#if defined(__F16C__)
            case SimdTypeF16C:
                return __builtin_cpu_supports("f16c") > 0;
                break;
#endif
#if defined(__SSE2__)
            case SimdTypeSSE2:
                return __builtin_cpu_supports("sse2") > 0;
                break;
#endif
#if defined(__AVX__)
            case SimdTypeAVX:
                return __builtin_cpu_supports("avx") > 0;
                break;
#endif
#if defined(__AVX2__)
            case SimdTypeAVX2:
                return __builtin_cpu_supports("avx2") > 0;
                break;
#endif
#if defined(__AVX512F__)
            case SimdTypeAVX512F:
                return __builtin_cpu_supports("avx512f") > 0;
                break;
#endif
#if defined(__AVX512VL__)
            case SimdTypeAVX512VL:
                return __builtin_cpu_supports("avx512vl") > 0;
                break;
#endif
#if defined(__AVX512BW__)
            case SimdTypeAVX512BW:
                return __builtin_cpu_supports("avx512bw") > 0;
                break;
#endif
#if defined(__AVX512DQ__)
            case SimdTypeAVX512DQ:
                return __builtin_cpu_supports("avx512dq") > 0;
                break;
#endif
#if defined(__AVX512CD__)
            case SimdTypeAVX512CD:
                return __builtin_cpu_supports("avx512cd") > 0;
                break;
#endif
#if defined(__AVX512ER__)
            case SimdTypeAVX512ER:
                return __builtin_cpu_supports("avx512er") > 0;
                break;
#endif
#if defined(__AVX512PF__)
            case SimdTypeAVX512PF:
                return __builtin_cpu_supports("avx512pf") > 0;
                break;
#endif
#if defined(__AVX512VBMI__)
            case SimdTypeAVX512VBMI:
                return __builtin_cpu_supports("avx512vbmi") > 0;
                break;
#endif
#if defined(__AVX512IFMA__)
            case SimdTypeAVX512IFMA:
                return __builtin_cpu_supports("avx512ifma") > 0;
                break;
#endif
#if defined(__AVX5124VNNIW__)
            case SimdTypeAVX5124VNNIW:
                return __builtin_cpu_supports("avx5124vnniw") > 0;
                break;
#endif
#if defined(__AVX5124FMAPS__)
            case SimdTypeAVX5124FMAPS:
                return __builtin_cpu_supports("avx5124fmaps") > 0;
                break;
#endif
#if defined(__AVX512VPOPCNTDQ__)
            case SimdTypeAVX512VPOPCNTDQ:
                return __builtin_cpu_supports("avx512vpopcntdq") > 0;
                break;
#endif
#if defined(__AVX512VBMI2__)
            case SimdTypeAVX512VBMI2:
                return __builtin_cpu_supports("avx512vbmi2") > 0;
                break;
#endif
#if defined(__AVX512VNNI__)
            case SimdTypeAVX512VNNI:
                return __builtin_cpu_supports("avx512vnni") > 0;
                break;
#endif
            default:
                break;
        }
        return false;
#elif defined(__GNUC__) && defined(__aarch64__)
        // use simde SSE2
        // ref: simd_common_intrin_include.h
        switch (type) {
            case SimdTypeSSE2:
                return true;
            default:
                return false;
        }
#else
#error "Unsupported platform."
        return false;
#endif
    }
    static bool isF16C() { return is(SimdTypeF16C); }
    static bool isSSE2() { return is(SimdTypeSSE2); }
    static bool isAVX2() { return is(SimdTypeAVX2); }
    static bool isAVX512() { return is(SimdTypeAVX512F); }
    static std::vector<char const *> getSupportedSimdTypes() {
        static constexpr char const *simdTypes[] = {"f16c",
                                                    "sse2",
                                                    "avx",
                                                    "avx2",
                                                    "avx512f",
                                                    "avx512vl",
                                                    "avx512bw",
                                                    "avx512dq",
                                                    "avx512cd",
                                                    "avx512er",
                                                    "avx512pf",
                                                    "avx512vbmi",
                                                    "avx512ifma",
                                                    "avx5124vnniw",
                                                    "avx5124fmaps",
                                                    "avx512vpopcntdq",
                                                    "avx512vbmi2",
                                                    "avx512vnni"};
        static constexpr int size = std::size(simdTypes);
        static_assert(size == SimdType::SimdTypeAVX512VNNI + 1, "The number of SIMD types is not correct.");
        std::vector<char const *> types;
        for (int i = 0; i < size; ++i) {
            if (is(static_cast<SimdType>(i))) {
                types.push_back(simdTypes[i]);
            }
        }
        return types;
    }
};
} // namespace NGT
