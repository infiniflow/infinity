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
import knn_result_handler;

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

Pair<Vector<u32>, Vector<f32>> LinScan::SearchBF(const SparseVecRef &query, u32 top_k) const {
    u32 result_n = std::min(top_k, row_num_);

    HashMap<u32, f32> scores;
    for (i32 i = 0; i < query.nnz_; ++i) {
        u32 indice = query.indices_[i];
        f32 val = query.data_[i];

        auto it = inverted_idx_.find(indice);
        if (it == inverted_idx_.end()) {
            continue;
        }
        const Vector<Posting> &posting_list = it->second;
        for (const auto &posting : posting_list) {
            scores[posting.doc_id_] += val * posting.val_;
        }
    }

    Vector<u32> res(result_n);
    Vector<f32> res_score(result_n);
    HeapResultHandler<CompareMin<f32, u32>> result_handler(1 /*query_n*/, result_n, res_score.data(), res.data());
    for (const auto &[row_id, score] : scores) {
        if (score < 0) {
            continue;
        }
        result_handler.AddResult(0 /*query_id*/, score, row_id);
    }
    result_handler.End(0 /*query_id*/);
    return {std::move(res), std::move(res_score)};
}

Pair<Vector<u32>, i32> LinScan::SearchKnn(const SparseVecRef &query, u32 top_k, i32 budget) const {
    if (budget <= 0) {
        return {};
    }
    u32 result_n = std::min(top_k, row_num_);

    Vector<u32> query_vec_idx(query.nnz_);
    std::iota(query_vec_idx.begin(), query_vec_idx.end(), 0);
    std::sort(query_vec_idx.begin(), query_vec_idx.end(), [&query](u32 i1, u32 i2) { return std::abs(query.data_[i1]) > std::abs(query.data_[i2]); });

    HashMap<u32, f32> scores;
    i32 cur_budget = 0;
    for (i32 budget_i = 0; cur_budget < budget && budget_i < query.nnz_; ++budget_i) {
        u32 indice = query.indices_[query_vec_idx[budget_i]];
        f32 val = query.data_[query_vec_idx[budget_i]];

        auto it = inverted_idx_.find(indice);
        if (it == inverted_idx_.end()) {
            continue;
        }
        const Vector<Posting> &posting_list = it->second;
        cur_budget += posting_list.size();
        for (const auto &posting : posting_list) {
            scores[posting.doc_id_] += val * posting.val_;
        }
    }

    Vector<u32> result(result_n);
    Vector<f32> result_score(result_n);
    HeapResultHandler<CompareMin<f32, u32>> result_handler(1 /*query_n*/, result_n, result_score.data(), result.data());
    for (const auto &[row_id, score] : scores) {
        if (score < 0) {
            continue;
        }
        result_handler.AddResult(0 /*query_id*/, score, row_id);
    }
    result_handler.EndWithoutSort(0 /*query_id*/);
    return {result, cur_budget};
}

} // namespace infinity