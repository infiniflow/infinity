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

#include "simd_init_h.h"
#include "NGT_CpuInfo_SimdType.h"

namespace infinity {

struct SupportedSimdTypes {
    std::vector<char const *> types_strs_ = NGT::CpuInfo::getSupportedSimdTypes();
    bool is_f16c_ = NGT::CpuInfo::isF16C();
    bool is_sse2_ = NGT::CpuInfo::isSSE2();
    bool is_avx2_ = NGT::CpuInfo::isAVX2();
    bool is_avx512_ = NGT::CpuInfo::isAVX512();
    bool is_avx512bw_ = NGT::CpuInfo::isAVX512BW();
};

const SupportedSimdTypes &GetSupportedSimdTypes() {
    static SupportedSimdTypes supported_simd_types;
    return supported_simd_types;
}

const std::vector<char const *> &GetSupportedSimdTypesList() { return GetSupportedSimdTypes().types_strs_; }

bool IsF16CSupported() { return GetSupportedSimdTypes().is_f16c_; }

bool IsSSE2Supported() { return GetSupportedSimdTypes().is_sse2_; }

bool IsAVX2Supported() { return GetSupportedSimdTypes().is_avx2_; }

bool IsAVX512Supported() { return GetSupportedSimdTypes().is_avx512_; }

bool IsAVX512BWSupported() { return GetSupportedSimdTypes().is_avx512bw_; }

} // namespace infinity
