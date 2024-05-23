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

#include <algorithm>
#include <vector>

module linscan_alg;

import stl;

namespace infinity {

void LinScan::Insert(const SparseVecRef &vec, u32 doc_id) {
    for (i32 i = 0; i < vec.nnz_; ++i) {
        u32 indice = vec.indices_[i];
        f32 val = vec.data_[i];
        Posting posting{doc_id, val};
        inverted_idx_[indice].push_back(posting);
    }
    ++row_num_;
}

Pair<Vector<u32>, Vector<f32>> LinScan::Query(const SparseVecRef &query, u32 top_k) const {
    auto scores = MakeUnique<f32[]>(row_num_);
    for (i32 i = 0; i < query.nnz_; ++i) {
        u32 indice = query.indices_[i];
        f32 val = query.data_[i];

        auto it = inverted_idx_.find(indice);
        if (it == inverted_idx_.end()) {
            continue;
        }
        const auto &posting_list = it->second;
        for (const auto &posting : posting_list) {
            scores[posting.doc_id_] += val * posting.val_;
        }
    }

    u32 result_n = std::min((u32)top_k, row_num_);
    Vector<u32> res(row_num_);
    std::iota(res.begin(), res.end(), 0);
    std::partial_sort(res.begin(), res.begin() + result_n, res.end(), [&scores](u32 i1, u32 i2) { return scores[i1] > scores[i2]; });
    res.resize(result_n);
    Vector<f32> res_score(result_n);
    std::transform(res.begin(), res.end(), res_score.begin(), [&scores](u32 i) { return scores[i]; });
    return {std::move(res), std::move(res_score)};
}

} // namespace infinity