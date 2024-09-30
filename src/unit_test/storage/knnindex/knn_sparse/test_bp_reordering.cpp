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

#include "gtest/gtest.h"
#include <cassert>
import base_test;

import stl;
import bp_reordering;
import third_party;
import infinity_exception;
import file_system;
import virtual_store;
import compilation_config;
import file_system_type;
import sparse_util;
import abstract_file_handle;
import local_file_handle;

using namespace infinity;

class BPReorderingTest : public BaseTest {
protected:
    i64 cost_func(i32 data_n, i32 query_n, const Vector<Vector<i32>> &fwd) {
        Vector<i32> last_doc_id(query_n, -1);
        i64 cost = 0;
        for (size_t doc_id = 0; doc_id < fwd.size(); ++doc_id) {
            for (const auto &term_id : fwd[doc_id]) {
                if (last_doc_id[term_id] != -1) {
                    i64 r = std::log2(doc_id - last_doc_id[term_id]);
                    cost += r;
                }
                last_doc_id[term_id] = doc_id;
            }
        }
        return cost;
    }

    i64 cost_func(i32 data_n, i32 query_n, const Vector<Vector<i32>> &fwd, const Vector<i32> &permu) {
        Vector<i32> last_doc_id(query_n, -1);
        i64 cost = 0;
        for (SizeT i = 0; i < permu.size(); ++i) {
            for (const auto &term_id : fwd[permu[i]]) {
                if (last_doc_id[term_id] != -1) {
                    i64 r = std::log2(i - last_doc_id[term_id]);
                    cost += r;
                }
                last_doc_id[term_id] = i;
            }
        }
        return cost;
    }

    Vector<i32> GetRandom(SizeT n, i32 min, i32 max) {
        Vector<i32> res;
        while (res.size() < n) {
            res.push_back(rand() % (max - min) + min);
        }
        return res;
    }

    Vector<i32> GetRandomNoRepeat(SizeT n, i32 min, i32 max) {
        if (n < 0 || max < min || max - min < i32(n)) {
            UnrecoverableError("max - min < n");
        }
        HashSet<i32> res;
        while (res.size() < n) {
            res.insert(rand() % (max - min) + min);
        }
        return Vector<i32>(res.begin(), res.end());
    }

    Vector<Vector<i32>> GenerateFwd(i32 data_n, i32 query_n, i32 edge_n, i32 m) {
        assert(data_n % m == 0);
        assert(edge_n % m == 0);
        i32 data_n1 = data_n / m;
        i32 edge_n1 = edge_n / m;
        Vector<Vector<i32>> fwd = GenerateFwd(data_n1, query_n, edge_n1);
        assert((i32)fwd.size() == data_n1);
        fwd.reserve(m);
        for (i32 i = 1; i < m; ++i) {
            for (i32 j = 0; j < data_n1; ++j) {
                fwd.push_back(fwd[j]);
            }
        }
        return fwd;
    }

    Vector<Vector<i32>> GenerateFwd(i32 data_n, i32 query_n, i32 edge_n) {
        Vector<Vector<i32>> fwd;

        Vector<i32> data_indices;
        while (true) {
            data_indices = GetRandom(data_n - 1, 0, edge_n);
            data_indices.push_back(0);
            data_indices.push_back(edge_n);
            std::sort(data_indices.begin(), data_indices.end());
            bool check = true;
            for (i32 i = 0; i < data_n; ++i) {
                auto diff = data_indices[i + 1] - data_indices[i];
                if (diff > query_n) {
                    check = false;
                    break;
                }
            }
            if (check) {
                break;
            }
            data_indices.clear();
        }

        fwd.reserve(data_n);
        for (i32 i = 0; i < data_n; ++i) {
            i32 qn = data_indices[i + 1] - data_indices[i];
            Vector<i32> posting = GetRandomNoRepeat(qn, 0, query_n);
            fwd.push_back(std::move(posting));
        }
        return fwd;
    };

    Vector<Vector<i32>> Fwd2Ivt(const Vector<Vector<i32>> &fwd, i32 query_n) {
        Vector<Vector<i32>> ivt(query_n);
        for (i32 i = 0; i < (i32)fwd.size(); ++i) {
            for (i32 j : fwd[i]) {
                ivt[j].push_back(i);
            }
        }
        return ivt;
    }
};

TEST_F(BPReorderingTest, test1) {
    i32 data_n = 1000;
    i32 query_n = 1000;
    i32 edge_n = data_n * query_n / 10;
    // i32 m = 100;
    // assert(data_n % m == 0);
    Vector<Vector<i32>> fwd = GenerateFwd(data_n, query_n, edge_n);

    BPReordering<i32, i32> bp(query_n);
    bp.set_terminate_length(4);
    for (i32 i = 0; i < data_n; ++i) {
        bp.AddDoc(&fwd[i]);
    }
    Vector<i32> reorder = bp();

    i64 old_cost = cost_func(data_n, query_n, fwd);
    i64 new_cost = cost_func(data_n, query_n, fwd, reorder);

//    std::cout << fmt::format("old_cost: {}, new_cost: {}\n", old_cost, new_cost);
    ASSERT_LE(new_cost, old_cost);
}

TEST_F(BPReorderingTest, test2) {
    // GTEST_SKIP() << "Skip this test. This program is not a test but for preprocessing data.";

    Path dataset_path = Path(test_data_path()) / "benchmark" / "splade" / "base_small.csr";
    auto [file_handle, status] = LocalStore::Open(dataset_path.string(), FileAccessMode::kRead);
    if (!status.ok()) {
        std::cout << String(status.message()) << std::endl;
        return;
    }
    auto dataset = SparseMatrix<f32, i32>::Load(*file_handle);
    i32 data_n = dataset.nrow_;
    // i32 data_n = 1000;
    Vector<Vector<i32>> fwd(data_n);
    for (i32 row_id = 0; row_id < data_n; ++row_id) {
        auto vec = dataset.at(row_id);
        // std::cout << fmt::format("Doc {}: ", row_id);
        fwd[row_id].reserve(vec.nnz_);
        for (i32 i = 0; i < vec.nnz_; ++i) {
            fwd[row_id].push_back(vec.indices_[i]);
            // std::cout << fmt::format("{} ", vec.indices_[i]);
        }
        // std::cout << std::endl;
    }
    // auto rng = std::default_random_engine{};
    // std::shuffle(fwd.begin(), fwd.end(), rng);

    i64 old_cost = cost_func(data_n, dataset.ncol_, fwd);

    BPReordering<i32, i32> bp(dataset.ncol_);
    for (i32 i = 0; i < data_n; ++i) {
        bp.AddDoc(&fwd[i]);
    }
    Vector<i32> reorder = bp();
    i64 new_cost = cost_func(data_n, dataset.ncol_, fwd, reorder);

//    std::cout << fmt::format("old_cost: {}, new_cost: {}\n", old_cost, new_cost);
    EXPECT_LE(new_cost, old_cost);
    file_handle->Close();
}
