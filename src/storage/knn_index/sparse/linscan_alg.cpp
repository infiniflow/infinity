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
    ++num_docs_;
}

Pair<Vector<u32>, Vector<f32>> LinScan::Query(const SparseVecRef &query, u32 top_k) const {
    auto scores = MakeUnique<f32[]>(num_docs_);
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

    SizeT result_n = std::min((SizeT)top_k, num_docs_);
    Vector<u32> res(result_n + 1);
    std::iota(res.begin(), res.begin() + result_n, 0);
    auto cmp = [&scores](u32 i1, u32 i2) { return scores[i1] > scores[i2]; };
    std::make_heap(res.begin(), res.begin() + result_n, cmp);
    for (SizeT i = result_n; i < num_docs_; ++i) {
        res[result_n] = i;
        std::push_heap(res.begin(), res.end(), cmp);
        std::pop_heap(res.begin(), res.end(), cmp);
    }
    res.pop_back();
    std::sort_heap(res.begin(), res.end(), cmp);
    Vector<f32> res_score(result_n);
    for (SizeT i = 0; i < result_n; ++i) {
        res_score[i] = scores[res[i]];
    }
    return {std::move(res), std::move(res_score)};
}

} // namespace infinity