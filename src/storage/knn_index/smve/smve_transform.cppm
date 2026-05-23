// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:smve_transform;

import :infinity_type;
import :mlas_matrix_multiply;

import std;

namespace infinity {

// SMVE transform: multi-vector → sparse vector
// Returns: (idx_values, data_values) as separate vectors

export struct SMVEResult {
    std::vector<u32> indices;
    std::vector<f32> values;
};

// Generate random projection matrix B: [dim × width], columns are unit vectors
export std::unique_ptr<f32[]> GenerateProjectionMatrix(u32 dim, u32 width);

// Transform a single multi-vector tensor to SMVE sparse vector
// tensor_ptr: [n_tokens × dim], query mode: sum pooling, doc mode: average pooling
export SMVEResult SMVETransform(const f32 *tensor_ptr,
                                u32 n_tokens,
                                u32 dim,
                                const f32 *projection_matrix,
                                u32 width,
                                u32 topk,
                                bool is_query);

} // namespace infinity
