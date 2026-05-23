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

module;

module infinity_core:smve_transform.impl;

import :smve_transform;

import std;

namespace infinity {

std::unique_ptr<f32[]> GenerateProjectionMatrix(u32 dim, u32 width) {
    // Generate random matrix with unit-norm columns
    // Using fixed seed for reproducibility
    auto mat = std::make_unique<f32[]>(dim * width);
    std::mt19937 rng(42); // fixed seed
    std::normal_distribution<f32> dist(0.0f, 1.0f);
    for (u32 j = 0; j < width; ++j) {
        f32 sum_sq = 0.0f;
        for (u32 i = 0; i < dim; ++i) {
            f32 val = dist(rng);
            mat[j * dim + i] = val;
            sum_sq += val * val;
        }
        f32 norm = std::sqrt(sum_sq);
        if (norm > 1e-12f) {
            for (u32 i = 0; i < dim; ++i) {
                mat[j * dim + i] /= norm;
            }
        }
    }
    return mat;
}

SMVEResult SMVETransform(const f32 *tensor_ptr, u32 n_tokens, u32 dim, const f32 *projection_matrix, u32 width, u32 topk, bool is_query) {
    // Step 1: Projection - compute scores = tensor × projection_matrix^T
    // scores: [n_tokens × width]
    auto scores = std::make_unique<f32[]>(n_tokens * width);
    matrixA_multiply_transpose_matrixB_output_to_C(tensor_ptr, projection_matrix, n_tokens, width, dim, scores.get());

    // Step 2: Per-token top-k sparsification
    // For each token, find top-k values and set rest to zero
    struct ScoreIdx {
        f32 score;
        u32 idx;
    };
    std::vector<f32> sparse_token(n_tokens * width, 0.0f);
    for (u32 t = 0; t < n_tokens; ++t) {
        f32 *row = scores.get() + t * width;
        std::vector<ScoreIdx> sorted;
        sorted.reserve(width);
        for (u32 j = 0; j < width; ++j) {
            sorted.push_back({row[j], j});
        }
        u32 k = std::min(topk, width);
        std::partial_sort(sorted.begin(), sorted.begin() + k, sorted.end(), [](const auto &a, const auto &b) { return a.score > b.score; });
        for (u32 j = 0; j < k; ++j) {
            sparse_token[t * width + sorted[j].idx] = sorted[j].score;
        }
    }

    // Step 3: Pooling
    // Query: sum across tokens
    // Doc:   average non-zero across tokens
    if (is_query) {
        // Sum pooling
        std::vector<u32> q_indices;
        std::vector<f32> q_values;
        q_indices.reserve(n_tokens * topk);
        q_values.reserve(n_tokens * topk);
        for (u32 j = 0; j < width; ++j) {
            f32 sum = 0.0f;
            for (u32 t = 0; t < n_tokens; ++t) {
                sum += sparse_token[t * width + j];
            }
            if (std::fabs(sum) > 1e-12f) {
                q_indices.push_back(j);
                q_values.push_back(sum);
            }
        }
        return {std::move(q_indices), std::move(q_values)};
    } else {
        // Average pooling: for each dimension, average non-zero values
        std::vector<u32> d_indices;
        std::vector<f32> d_values;
        d_indices.reserve(width);
        d_values.reserve(width);
        for (u32 j = 0; j < width; ++j) {
            f32 sum = 0.0f;
            u32 cnt = 0;
            for (u32 t = 0; t < n_tokens; ++t) {
                f32 v = sparse_token[t * width + j];
                if (std::fabs(v) > 1e-12f) {
                    sum += v;
                    ++cnt;
                }
            }
            if (cnt > 0) {
                d_indices.push_back(j);
                d_values.push_back(sum / static_cast<f32>(cnt));
            }
        }
        return {std::move(d_indices), std::move(d_values)};
    }
}

} // namespace infinity
