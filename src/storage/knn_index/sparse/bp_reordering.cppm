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

#include <iostream>
#include <vector>

export module bp_reordering;

import stl;
import third_party;

namespace infinity {

export class BPReordering {
public:
    BPReordering(i32 query_n, const Vector<Vector<i32>> &fwd, i32 iter_n = 20) : query_n_(query_n), fwd_(fwd), iter_n_(iter_n) {}

    Vector<i32> operator()() {
        i32 data_n = fwd_.size();
        Vector<i32> ret(data_n);
        std::iota(ret.begin(), ret.end(), 0);
        inner(0, data_n, ret);
        return ret;
    }

private:
    void inner(i32 start, i32 end, Vector<i32> &res) {
        if (end - start <= 1) {
            return;
        }
        i32 mid = (start + end) / 2;
        i32 n1 = mid - start;
        i32 n2 = end - mid;

        for (i32 iter_i = 0; iter_i < iter_n_; ++iter_i) {
            HashMap<i32, Pair<i32, i32>> degs;
            for (i32 i = start; i < end; ++i) {
                i32 data_i = res[i];
                for (i32 query_i : fwd_[data_i]) {
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
            HashMap<i32, Pair<f32, f32>> diff;
            for (const auto &[query_i, deg] : degs) {
                const auto &[deg1, deg2] = deg;
                diff[query_i] = {cal_gain(deg1, n1, deg2, n2), cal_gain(deg2, n2, deg1, n1)};
            }

            Vector<f32> gains(end - start, 0.0);
            for (i32 i = start; i < end; ++i) {
                i32 data_i = res[i];
                for (i32 query_i : fwd_[data_i]) {
                    const auto &[diff1, diff2] = diff[query_i];
                    if (i < mid) {
                        gains[i - start] += diff1;
                    } else {
                        gains[i - start] += diff2;
                    }
                }
            }

            Vector<i32> idx1(n1);
            Vector<i32> idx2(n2);
            std::iota(idx1.begin(), idx1.end(), 0);
            std::iota(idx2.begin(), idx2.end(), 0);
            std::sort(idx1.begin(), idx1.end(), [&](i32 i1, i32 i2) { return gains[i1] > gains[i2]; });
            std::sort(idx2.begin(), idx2.end(), [&](i32 i1, i32 i2) { return gains[i1 + n1] > gains[i2 + n1]; });

            bool converge = true;
            for (i32 i = 0; i < std::min(n1, n2); ++i) {
                i32 &data_i1 = res[idx1[i] + start];
                i32 &data_i2 = res[idx2[i] + mid];
                f32 gain1 = gains[idx1[i]];
                f32 gain2 = gains[idx2[i] + n1];
                if (gain1 + gain2 > 0) {
                    converge = false;
                    std::cout << fmt::format("Swap {} with {}", data_i1, data_i2) << std::endl;
                    std::swap(data_i1, data_i2);
                } else {
                    break;
                }
            }
            if (converge) {
                break;
            }
        }

        inner(start, mid, res);
        inner(mid, end, res);
    }

private:
    i32 query_n_;
    const Vector<Vector<i32>> &fwd_;

    i32 iter_n_;
};

} // namespace infinity
