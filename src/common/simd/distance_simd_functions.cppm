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
export module distance_simd_functions;
import stl;

namespace infinity {

export f32 L2Distance_common(const f32 *x, const f32 *y, SizeT d);

export f32 IPDistance_common(const f32 *x, const f32 *y, SizeT d);

export f32 CosineDistance_common(const f32 *x, const f32 *y, SizeT d);

export f32 HammingDistance_common(const u8 *x, const u8 *y, SizeT d);

#if defined(__AVX2__)
export f32 L2Distance_avx2(const f32 *vector1, const f32 *vector2, SizeT dimension);

export f32 IPDistance_avx2(const f32 *vector1, const f32 *vector2, SizeT dimension);

export f32 CosineDistance_avx2(const f32 *vector1, const f32 *vector2, SizeT dimension);

export f32 HammingDistance_avx2(const u8 *vector1, const u8 *vector2, SizeT dimension);
#endif

#if defined(__SSE2__)
export f32 HammingDistance_sse2(const u8 *vector1, const u8 *vector2, SizeT dimesion);
#endif

} // namespace infinity
