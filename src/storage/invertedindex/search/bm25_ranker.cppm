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

export module bm25_ranker;

import stl;

namespace infinity {
export class BM25Ranker {
public:
    BM25Ranker(u64 total_df);
    ~BM25Ranker() = default;

    void AddTermParam(u64 tf, u64 df, double avg_column_len, u64 column_len, float weight);

    float GetScore() { return score_; }

private:
    float score_{0};
    i64 total_df_{0};
};
} // namespace infinity