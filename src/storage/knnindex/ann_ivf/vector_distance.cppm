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
#include <type_traits>
import stl;
import some_simd_functions;

export module vector_distance;

namespace infinity {
export template <typename DiffType, typename ElemType1, typename ElemType2>
DiffType L2Distance(const ElemType1 *vector1, const ElemType2 *vector2, u32 dimension) {
    if constexpr (std::is_same_v<ElemType1, f32> && std::is_same_v<ElemType2, f32>) {
        return L2Distance_simd(vector1, vector2, dimension);
    } else {
        DiffType distance{};
        for (u32 i = 0; i < dimension; ++i) {
            auto diff = (DiffType)(vector1[i] - vector2[i]);
            distance += diff * diff;
        }
        return distance;
    }
}

export template <typename DiffType, typename ElemType1, typename ElemType2>
DiffType IPDistance(const ElemType1 *vector1, const ElemType2 *vector2, u32 dimension) {
    if constexpr (std::is_same_v<ElemType1, f32> && std::is_same_v<ElemType2, f32>) {
        return IPDistance_simd(vector1, vector2, dimension);
    } else {
        DiffType distance{};
        for (u32 i = 0; i < dimension; ++i) {
            distance += ((DiffType)vector1[i]) * ((DiffType)vector2[i]);
        }
        return distance;
    }
}

} // namespace infinity
