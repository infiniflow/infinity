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

struct BPReorderContext {
public:
    BPReorderContext(SizeT term_num, SizeT doc_num)
        : ldegs(term_num), rdegs(term_num), term_gains_toright_(term_num), term_gains_toleft_(term_num), term_gains_toright_valid_(term_num),
          term_gains_toleft_valid_(term_num), gains_(doc_num), gains_permu_(doc_num) {}

public:
    Vector<i32> ldegs;
    Vector<i32> rdegs;

    Vector<float> term_gains_toright_;
    Vector<float> term_gains_toleft_;
    Vector<bool> term_gains_toright_valid_;
    Vector<bool> term_gains_toleft_valid_;

    Vector<float> gains_;
    Vector<i32> gains_permu_;
};

export template <typename IdxType, typename DocID>
class BPReordering {
public:
    BPReordering(i32 query_n) : query_n_(query_n), terminate_length_(32), iter_n_(20), log_thread_n_(4) {}

    void set_terminate_length(i32 terminate_length) { terminate_length_ = terminate_length; }
    void set_iter_n(i32 iter_n) { iter_n_ = iter_n; }
    void set_log_thread_n(i32 log_thread_n) { log_thread_n_ = log_thread_n; }

    void AddDoc(const Vector<IdxType> *terms) { fwd_.push_back(terms); }

    Vector<DocID> operator()() {
        SizeT data_n = fwd_.size();
        Vector<DocID> permutation(data_n);
        std::iota(permutation.begin(), permutation.end(), 0);
        Thread([&] { Work(0 /*start*/, data_n, 0 /*depth*/, permutation); }).join();
        return permutation;
    }

private:
    void Work(i32 start, i32 end, SizeT depth, Vector<DocID> &permutation) {
        std::sort(permutation.begin() + start, permutation.begin() + end);
        if (end - start <= terminate_length_) {
            return;
        }
        thread_local BPReorderContext ctx(query_n_, fwd_.size());

        i32 mid = (start + end) / 2;
        Bisection(start, mid, end, ctx, permutation, depth);
        if (depth < log_thread_n_) {
            auto t = Thread([&] { Work(start, mid, depth + 1, permutation); });
            Work(mid, end, depth + 1, permutation);
            t.join();
        } else {
            Work(start, mid, depth + 1, permutation);
            Work(mid, end, depth + 1, permutation);
        }
    }

    void Bisection(i32 start, i32 mid, i32 end, BPReorderContext &ctx, Vector<DocID> &permutation, SizeT depth) {
        std::fill(ctx.ldegs.begin(), ctx.ldegs.end(), 0);
        std::fill(ctx.rdegs.begin(), ctx.rdegs.end(), 0);
        std::fill(ctx.term_gains_toright_valid_.begin(), ctx.term_gains_toright_valid_.end(), false);
        std::fill(ctx.term_gains_toleft_valid_.begin(), ctx.term_gains_toleft_valid_.end(), false);

        for (i32 i = start; i < end; ++i) {
            DocID data_i = permutation[i];
            for (auto query_i : *fwd_[data_i]) {
                if (i < mid) {
                    ++ctx.ldegs[query_i];
                } else {
                    ++ctx.rdegs[query_i];
                }
            }
        }

        for (i32 iter_i = 0; iter_i < iter_n_; ++iter_i) {
            bool swap = false;

            ComputeGains(start, mid, end, ctx, permutation, depth);
            std::iota(ctx.gains_permu_.begin() + start, ctx.gains_permu_.begin() + end, start);

            std::sort(ctx.gains_permu_.begin() + start, ctx.gains_permu_.begin() + mid, [&ctx](SizeT i, SizeT j) {
                return ctx.gains_[i] > ctx.gains_[j];
            });
            std::sort(ctx.gains_permu_.begin() + mid, ctx.gains_permu_.begin() + end, [&ctx](SizeT i, SizeT j) {
                return ctx.gains_[i] > ctx.gains_[j];
            });

            for (i32 i = start, j = mid; i < mid; ++i, ++j) {
                i32 li = ctx.gains_permu_[i];
                i32 ri = ctx.gains_permu_[j];
                if (ctx.gains_[li] + ctx.gains_[ri] <= 0) {
                    break;
                }
                swap = true;
                for (const auto &term : *fwd_[permutation[li]]) {
                    ctx.ldegs[term]--;
                    ctx.rdegs[term]++;
                    ctx.term_gains_toright_valid_[term] = false;
                    ctx.term_gains_toleft_valid_[term] = false;
                }
                for (const auto &term : *fwd_[permutation[ri]]) {
                    ctx.rdegs[term]--;
                    ctx.ldegs[term]++;
                    ctx.term_gains_toright_valid_[term] = false;
                    ctx.term_gains_toleft_valid_[term] = false;
                }

                std::swap(permutation[li], permutation[ri]);
            }

            if (!swap) {
                iter_i = iter_n_;
            }
        }
    }

    void ComputeGains(SizeT start, SizeT mid, SizeT end, BPReorderContext &ctx, const Vector<DocID> &permutation, SizeT depth) const {
        float left_log_n = std::log2(mid - start);
        float right_log_n = std::log2(end - mid);
        auto work = [&](SizeT i1, SizeT i2) {
            for (SizeT i = i1; i < i2; ++i) {
                if (i < mid) {
                    ctx.gains_[i] = ComputeGain(permutation[i],
                                                left_log_n,
                                                right_log_n,
                                                ctx.ldegs,
                                                ctx.rdegs,
                                                ctx.term_gains_toright_,
                                                ctx.term_gains_toright_valid_);
                } else {
                    ctx.gains_[i] = ComputeGain(permutation[i],
                                                right_log_n,
                                                left_log_n,
                                                ctx.rdegs,
                                                ctx.ldegs,
                                                ctx.term_gains_toleft_,
                                                ctx.term_gains_toleft_valid_);
                }
            }
        };
        if (depth < log_thread_n_) {
            Vector<Thread> threads;
            SizeT thread_n = 1 << (log_thread_n_ - depth);
            SizeT step = (end - start) / thread_n;
            for (SizeT i = 0; i < thread_n; ++i) {
                threads.emplace_back(work, start + i * step, i == thread_n - 1 ? end : start + (i + 1) * step);
            }
            for (auto &t : threads) {
                t.join();
            }
        } else {
            work(start, end);
        }
    }

    float ComputeGain(DocID doc_id,
                      float from_log_n,
                      float to_log_n,
                      const Vector<i32> &from_degs,
                      const Vector<i32> &to_degs,
                      Vector<float> &term_gains,
                      Vector<bool> &term_gains_valid) const {
        float gain = 0;
        for (const auto &term_id : *fwd_[doc_id]) {
            if (!term_gains_valid[term_id]) {
                term_gains[term_id] = ComputeTermGain(from_degs[term_id], from_log_n, to_degs[term_id], to_log_n);

                term_gains_valid[term_id] = true;
            }
            gain += term_gains[term_id];
        }
        return gain;
    }

    static float ComputeTermGain(i32 deg1, float log_n1, i32 deg2, float log_n2) {
        return ComputeTermCost(deg1, log_n1, deg2, log_n2) - ComputeTermCost(deg1 - 1, log_n1, deg2 + 1, log_n2);
    }

    // cost = deg * log2(n / (deg + 1)) = deg * log2(n) - deg * log2(deg + 1)
    static float ComputeTermCost(i32 deg1, float log_n1, i32 deg2, float log_n2) {
        return deg1 * log_n1 - deg1 * std::log2(deg1 + 1) + deg2 * log_n2 - deg2 * std::log2(deg2 + 1);
    }

private:
    Vector<const Vector<IdxType> *> fwd_;
    i32 query_n_;
    i32 terminate_length_;
    i32 iter_n_;
    SizeT log_thread_n_;
};

} // namespace infinity
