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

import stl;
import bp_reordering;
import third_party;
import infinity_exception;

using namespace infinity;

class BPReorderingTest : public BaseTest {
protected:
    f32 cost_func(i32 data_n, i32 query_n, const Vector<Vector<i32>> &fwd, const Vector<i32> &reorder) {
        Vector<Vector<i32>> ivt = Fwd2Ivt(fwd, query_n);
        f32 cost = 0;
        for (auto &posting : ivt) {
            if (posting.empty()) {
                continue;
            }
            std::sort(posting.begin(), posting.end(), [&](i32 a, i32 b) { return reorder[a] < reorder[b]; });
            for (SizeT i = 0; i < posting.size() - 1; ++i) {
                float r = std::log2(reorder[posting[i + 1]] - reorder[posting[i]]);
                cost += r;
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
        Vector<Vector<i32>> fwd = GenerateFwd(data_n / m, query_n, edge_n / m);
        assert((i32)fwd.size() == edge_n / m);
        for (i32 i = 1; i < m; ++i) {
            for (i32 j = 0; j < edge_n / m; ++j) {
                fwd.push_back(fwd[j]);
            }
        }
        return fwd;
    }

    Vector<Vector<i32>> GenerateFwd(i32 data_n, i32 query_n, i32 edge_n) {
        Vector<Vector<i32>> fwd;

        Vector<i32> data_indices = GetRandom(data_n - 1, 0, edge_n);
        data_indices.push_back(0);
        data_indices.push_back(edge_n);
        std::sort(data_indices.begin(), data_indices.end());
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
    i32 data_n = 9;
    i32 query_n = 5;
    Vector<Vector<i32>> fwd(9);
    for (i32 i = 0; i < 9; ++i) {
        fwd[i].push_back(i % 3);
        fwd[i].push_back(3 + i % 2);
    }

    Vector<i32> old_order(data_n);
    std::iota(old_order.begin(), old_order.end(), 0);
    f32 old_cost = cost_func(data_n, query_n, fwd, old_order);

    BPReordering bp(query_n, fwd);
    Vector<i32> reorder = bp();
    f32 new_cost = cost_func(data_n, query_n, fwd, reorder);

    // Vector<i32> gt_reorder = {0, 1, 4, 2, 5, 6, 3, 7, 8};

    std::cout << fmt::format("old_cost: {}, new_cost: {}\n", old_cost, new_cost);
}

TEST_F(BPReorderingTest, test2) {
    i32 data_n = 100;
    i32 query_n = 10;
    i32 edge_n = data_n * query_n / 10;
    i32 m = 10;
    assert(data_n % m == 0);
    Vector<Vector<i32>> fwd = GenerateFwd(data_n, query_n, edge_n);

    BPReordering bp(query_n, fwd, 20);
    Vector<i32> reorder = bp();

    Vector<i32> old_order(data_n);
    std::iota(old_order.begin(), old_order.end(), 0);
    f32 old_cost = cost_func(data_n, query_n, fwd, old_order);
    f32 new_cost = cost_func(data_n, query_n, fwd, reorder);

    std::cout << fmt::format("old_cost: {}, new_cost: {}\n", old_cost, new_cost);
    ASSERT_LT(new_cost, old_cost);
}
