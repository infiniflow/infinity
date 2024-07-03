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

#include <cassert>
#include <iostream>
#include <vector>

export module bp_reordering;

import stl;
import third_party;

namespace infinity {

export template <typename IdxType, typename DocID>
class BPReordering {
public:
    void AddDoc(const Vector<IdxType> *terms) { fwd_.push_back(terms); }

    Vector<DocID> operator()(i32 query_n, i32 max_depth = 0, i32 iter_n = 20) {
        query_n_ = query_n;
        if (max_depth == 0) {
            max_depth = std::max(0, static_cast<i32>(std::log2(fwd_.size()) - 4));
        }
        max_depth_ = max_depth;
        iter_n_ = iter_n;

        SizeT data_n = fwd_.size();
        res_map_.resize(data_n);
        std::iota(res_map_.begin(), res_map_.end(), 0);
        work(0, data_n, 0);
        return std::move(res_map_);
    }

private:
    void work(i32 start, i32 end, i32 depth) {
        if (end - start <= 1 || depth >= max_depth_) {
            return;
        }
        i32 mid = (start + end) / 2;
        inner(start, mid, end);
        if (depth >= 4) {
            work(start, mid, depth + 1);
            work(mid, end, depth + 1);
        } else {
            auto t = Thread([&] { work(start, mid, depth + 1); });
            work(mid, end, depth + 1);
            t.join();
        }
    }

    void inner(i32 start, i32 mid, i32 end) {
        i32 n1 = mid - start;
        i32 n2 = end - mid;
        Vector<Pair<i32, i32>> degs(query_n_, {0, 0});
        Vector<Pair<f32, f32>> gain_diff(query_n_, {0, 0});
        for (i32 i = start; i < end; ++i) {
            DocID data_i = res_map_[i];
            for (IdxType query_i : *fwd_[data_i]) {
                if (i < mid) {
                    ++degs[query_i].first;
                } else {
                    ++degs[query_i].second;
                }
            }
        }
        auto cost = [](i32 deg1, i32 n1, i32 deg2, i32 n2) {
            return deg1 * std::log2(static_cast<f32>(n1) / (deg1 + 1)) + deg2 * std::log2(static_cast<f32>(n2) / (deg2 + 1));
        };
        auto cal_gain = [&](i32 deg_from, i32 n_from, i32 deg_to, i32 n_to) {
            f32 cost_old = cost(deg_from, n_from, deg_to, n_to);
            f32 cost_new = cost(deg_from - 1, n_from, deg_to + 1, n_to);
            return cost_old - cost_new;
        };
        for (i32 query_i = 0; query_i < query_n_; ++query_i) {
            const auto &[deg1, deg2] = degs[query_i];
            if (deg1 || deg2) {
                gain_diff[query_i] = {cal_gain(deg1, n1, deg2, n2), cal_gain(deg2, n2, deg1, n1)};
            }
        }

        for (i32 iter_i = 0; iter_i < iter_n_; ++iter_i) {
            Vector<f32> gains(end - start, 0.0);
            for (i32 i = start; i < end; ++i) {
                DocID data_i = res_map_[i];
                for (IdxType query_i : *fwd_[data_i]) {
                    const auto &[gains1, gains2] = gain_diff[query_i];
                    if (i < mid) {
                        gains[i - start] += gains1;
                    } else {
                        gains[i - start] += gains2;
                    }
                }
            }

            Vector<i32> idx1(n1);
            Vector<i32> idx2(n2);
            std::iota(idx1.begin(), idx1.end(), 0);
            std::iota(idx2.begin(), idx2.end(), 0);
            std::sort(idx1.begin(), idx1.end(), [&](i32 i1, i32 i2) { return gains[i1] > gains[i2]; });
            std::sort(idx2.begin(), idx2.end(), [&](i32 i1, i32 i2) { return gains[i1 + n1] > gains[i2 + n1]; });

            HashSet<i32> recalulate;
            for (i32 i = 0; i < std::min(n1, n2); ++i) {
                DocID &data_i1 = res_map_[idx1[i] + start];
                DocID &data_i2 = res_map_[idx2[i] + mid];
                f32 gain1 = gains[idx1[i]];
                f32 gain2 = gains[idx2[i] + n1];
                if (gain1 + gain2 <= 0) {
                    break;
                }
                for (IdxType query_i : *fwd_[data_i1]) {
                    --degs[query_i].first;
                    ++degs[query_i].second;
                    recalulate.insert(query_i);
                }
                for (IdxType query_i : *fwd_[data_i2]) {
                    --degs[query_i].second;
                    ++degs[query_i].first;
                    recalulate.insert(query_i);
                }
                // std::cout << fmt::format("Swap {} with {}", data_i1, data_i2) << std::endl;
                std::swap(data_i1, data_i2);
            }
            if (recalulate.empty()) {
                break;
            }
            for (i32 query_i : recalulate) {
                const auto &[deg1, deg2] = degs[query_i];
                if (deg1 || deg2) {
                    gain_diff[query_i] = {cal_gain(deg1, n1, deg2, n2), cal_gain(deg2, n2, deg1, n1)};
                } else {
                    gain_diff[query_i] = {0, 0};
                }
            }
        }
    }

private:
    Vector<const Vector<IdxType> *> fwd_;

    i32 query_n_;
    i32 max_depth_;
    i32 iter_n_;
    Vector<DocID> res_map_;
};

} // namespace infinity
