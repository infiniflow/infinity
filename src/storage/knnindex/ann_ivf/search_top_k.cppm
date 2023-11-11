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

#include <limits>
#include <type_traits>

import stl;
import vector_distance;
import search_top_1_sgemm;

export module search_top_k;

namespace infinity {

template <typename TypeX, typename TypeY, typename ID, typename DistType>
void search_top_1_simple_with_dis(u32 dimension, u32 nx, const TypeX *x, u32 ny, const TypeY *y, ID *labels, DistType *distances) {
    for (u32 i = 0; i < nx; ++i) {
        DistType min_dist = std::numeric_limits<DistType>::max();
        u32 min_idx = 0;
        for (u32 j = 0; j < ny; ++j) {
            DistType dist = L2Distance<DistType>(x + i * dimension, y + j * dimension, dimension);
            if (dist < min_dist) {
                min_dist = dist;
                min_idx = j;
            }
        }
        distances[i] = min_dist;
        labels[i] = min_idx;
    }
}

export template <typename TypeX, typename TypeY, typename ID, typename DistType>
void search_top_1_with_dis(u32 dimension, u32 nx, const TypeX *x, u32 ny, const TypeY *y, ID *labels, DistType *distances) {
    if constexpr (std::is_same_v<DistType, TypeX> && std::is_same_v<DistType, TypeY> && std::is_same_v<DistType, f32>) {
        search_top_1_with_sgemm(dimension, nx, x, ny, y, labels, distances);
    } else {
        search_top_1_simple_with_dis(dimension, nx, x, ny, y, labels, distances);
    }
}

template <typename DistType, typename TypeX, typename TypeY, typename ID>
void search_top_1_simple_without_dis(u32 dimension, u32 nx, const TypeX *x, u32 ny, const TypeY *y, ID *labels) {
    for (u32 i = 0; i < nx; ++i) {
        DistType min_dist = std::numeric_limits<DistType>::max();
        u32 min_idx = 0;
        for (u32 j = 0; j < ny; ++j) {
            DistType dist = L2Distance<DistType>(x + i * dimension, y + j * dimension, dimension);
            if (dist < min_dist) {
                min_dist = dist;
                min_idx = j;
            }
        }
        labels[i] = min_idx;
    }
}

export template <typename DistType, typename TypeX, typename TypeY, typename ID>
void search_top_1_without_dis(u32 dimension, u32 nx, const TypeX *x, u32 ny, const TypeY *y, ID *labels) {
    if constexpr (std::is_same_v<DistType, TypeX> && std::is_same_v<DistType, TypeY> && std::is_same_v<DistType, f32>) {
        search_top_1_with_sgemm(dimension, nx, x, ny, y, labels);
    } else {
        search_top_1_simple_without_dis<DistType>(dimension, nx, x, ny, y, labels);
    }
}

} // namespace infinity