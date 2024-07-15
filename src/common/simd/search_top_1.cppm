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

export module search_top_1;

import stl;
import simd_init;
import search_top_1_sgemm;

namespace infinity {

export template <typename DistType, typename TypeX, typename TypeY, typename ID>
void search_top_1_without_dis(u32 dimension, u32 nx, const TypeX *x, u32 ny, const TypeY *y, ID *labels) {
    if constexpr (std::is_same_v<DistType, TypeX> && std::is_same_v<DistType, TypeY> && std::is_same_v<DistType, f32>) {
#if defined(__AVX2__)
        if (IsAVX2Supported()) {
            return search_top_1_without_dis_avx2(dimension, nx, x, ny, y, labels);
        }
#endif
#if defined(__SSE2__)
        if (IsSSE2Supported()) {
            return search_top_1_without_dis_sse2(dimension, nx, x, ny, y, labels);
        }
#endif
    }
    return search_top_1_simple_without_dis<DistType>(dimension, nx, x, ny, y, labels);
}

} // namespace infinity