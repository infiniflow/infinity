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

#include "unit_test/base_test.h"
#include <cassert>
#include <random>

import stl;
import bp_reordering;
import third_party;
import infinity_exception;
import file_system;
import local_file_system;
import compilation_config;
import file_system_type;
import sparse_util;

using namespace infinity;

class BPReorderingTest : public BaseTest {
protected:
    i64 cost_func(i32 data_n, i32 query_n, const Vector<Vector<i32>> &fwd) {
        Vector<i32> last_doc_id(query_n, -1);
        i64 cost = 0;
        for (int doc_id = 0; doc_id < fwd.size(); ++doc_id) {
            for (const auto &term_id : fwd[doc_id]) {
                if (last_term_[term_id] != -1) {
                    i64 r = std::log2(doc_id - last_doc_id[term_id]);
                    cost += r;
                }
                last_doc_id[term_id] = doc_;
            }
        }
        return cost;
    }

    i64 cost_func(i32 data_n, i32 query_n, const Vector<Vector<i32>> &fwd, const Vector<i32> &permu) {
        Vector<i32> last_doc_id(query_n, -1);
        i64 cost = 0;
        for (i32 doc_id : permu) {
            for (const auto &term_id : fwd[doc_id]) {
                if (last_term_[term_id] != -1) {
                    i64 r = std::log2(doc_id - last_doc_id[term_id]);
                    cost += r;
                }
                last_doc_id[term_id] = doc_;
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
    Vector<Pair<i32, i32>> term_infos{{3, 0}, {2, 3}, {4, 5}};
    i32 query_n = 0;
    for (const auto &[term_num, term_off] : term_infos) {
        query_n += term_num;
    }
    for (i32 data_n = 10; data_n < 100; ++data_n) {
        Vector<Vector<i32>> fwd(data_n);
        for (i32 i = 0; i < data_n; ++i) {
            for (const auto &[term_num, term_off] : term_infos) {
                fwd[i].push_back(term_off + i % term_num);
            }
        }
        // auto rng = std::default_random_engine{};
        // std::shuffle(fwd.begin(), fwd.end(), rng);

        i64 old_cost = cost_func(data_n, query_n, fwd);

        BPReordering<i32, i32> bp;
        for (i32 i = 0; i < data_n; ++i) {
            bp.AddDoc(&fwd[i]);
        }
        Vector<i32> reorder = bp(query_n, std::log2(data_n));
        i64 new_cost = cost_func(data_n, query_n, fwd, reorder);

        // Vector<i32> gt_reorder = {0, 1, 4, 2, 5, 6, 3, 7, 8};

        std::cout << fmt::format("data_n: {}, old_cost: {}, new_cost: {}\n", data_n, old_cost, new_cost);
    }
}

TEST_F(BPReorderingTest, test2) {
    i32 data_n = 1000;
    i32 query_n = 1000;
    i32 edge_n = data_n * query_n / 10;
    // i32 m = 100;
    // assert(data_n % m == 0);
    Vector<Vector<i32>> fwd = GenerateFwd(data_n, query_n, edge_n);

    BPReordering<i32, i32> bp;
    for (i32 i = 0; i < data_n; ++i) {
        bp.AddDoc(&fwd[i]);
    }
    Vector<i32> reorder = bp(query_n);

    i64 old_cost = cost_func(data_n, query_n, fwd);
    i64 new_cost = cost_func(data_n, query_n, fwd, reorder);

    std::cout << fmt::format("old_cost: {}, new_cost: {}\n", old_cost, new_cost);
    ASSERT_LE(new_cost, old_cost);
}

TEST_F(BPReorderingTest, test3) {
    GTEST_SKIP() << "Skip this test. This program is not a test but for preprocessing data.";

    LocalFileSystem fs;
    Path dataset_path = Path(test_data_path()) / "benchmark" / "splade" / "base_small_shuffled.csr";
    auto [file_handler, status] = fs.OpenFile(dataset_path.string(), FileFlags::READ_FLAG, FileLockType::kNoLock);
    if (!status.ok()) {
        std::cout << String(status.message()) << std::endl;
        return;
    }
    auto dataset = SparseMatrix<f32, i32>::Load(*file_handler);
    i32 data_n = dataset.nrow_;
    // i32 data_n = 1000;
    Vector<Vector<i32>> fwd(data_n);
    for (i32 row_id = 0; row_id < data_n; ++row_id) {
        auto vec = dataset.at(row_id);
        // std::cout << fmt::format("Doc {}: ", row_id);
        for (i32 i = 0; i < vec.nnz_; ++i) {
            fwd[row_id].push_back(vec.indices_[i]);
            // std::cout << fmt::format("{} ", vec.indices_[i]);
        }
        // std::cout << std::endl;
    }
    // auto rng = std::default_random_engine{};
    // std::shuffle(fwd.begin(), fwd.end(), rng);

    i64 old_cost = cost_func(data_n, dataset.ncol_, fwd);

    BPReordering<i32, i32> bp;
    for (i32 i = 0; i < data_n; ++i) {
        bp.AddDoc(&fwd[i]);
    }
    Vector<i32> reorder = bp(dataset.ncol_);
    i64 new_cost = cost_func(data_n, dataset.ncol_, fwd, reorder);

    std::cout << fmt::format("old_cost: {}, new_cost: {}\n", old_cost, new_cost);
}
