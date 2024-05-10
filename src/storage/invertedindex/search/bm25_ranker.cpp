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

#include <cmath>

module bm25_ranker;

import stl;
import index_defines;
import third_party;

namespace infinity {

constexpr float k1 = 1.2F;
constexpr float b = 0.75F;

BM25Ranker::BM25Ranker(u64 total_df) : total_df_(std::max(total_df, 1UL)) {}

void BM25Ranker::AddTermParam(u64 tf, u64 df, float avg_column_len, u32 column_len, float weight) {
    float smooth_idf = std::log(1.0F + (total_df_ - df + 0.5F) / (df + 0.5F));
    float smooth_tf = (k1 + 1.0F) * tf / (tf + k1 * (1.0F - b + b * column_len / avg_column_len));
    score_ += smooth_idf * smooth_tf * weight;
}

void BM25Ranker::AddPhraseParam(tf_t tf, u64 df, float avg_column_len, u32 column_len, float weight) {
    float smooth_idf = std::log(1.0F + (total_df_ - df + 0.5F) / (df + 0.5F));
    float smooth_tf = (k1 + 1.0F) * tf / (tf + k1 * (1.0F - b + b * column_len / avg_column_len));
    score_ += smooth_idf * smooth_tf * weight;
}

} // namespace infinity
