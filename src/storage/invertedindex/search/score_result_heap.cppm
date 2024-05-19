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

export module fulltext_score_result_heap;
import stl;
import internal_types;

namespace infinity {

template <typename ScoreType, typename ID>
struct HeapCompare {
    static bool Less(ScoreType a, ScoreType b, ID i, ID j) { return (a < b) or ((a == b) and (i > j)); }
    static constexpr ScoreType MinValue() { return std::numeric_limits<ScoreType>::lowest(); }
};

export class FullTextScoreResultHeap {
    using ScoreType = float;
    using ID = RowID;
    using Compare = HeapCompare<ScoreType, ID>;
    ScoreType *score_ = nullptr;
    ID *id_ = nullptr;
    const u32 top_k_ = 0;
    u32 size_ = 0;

    inline void HeapifyDown(const u32 size, u32 index) {
        if (index == 0 || (index << 1) > size) {
            return;
        }
        auto tmp_d = score_[index];
        auto tmp_i = id_[index];
        for (u32 sub; (sub = (index << 1)) <= size; index = sub) {
            if (sub + 1 <= size and Compare::Less(score_[sub + 1], score_[sub], id_[sub + 1], id_[sub])) {
                ++sub;
            }
            if (!Compare::Less(score_[sub], tmp_d, id_[sub], tmp_i)) {
                break;
            }
            score_[index] = score_[sub];
            id_[index] = id_[sub];
        }
        score_[index] = tmp_d;
        id_[index] = tmp_i;
    }

public:
    FullTextScoreResultHeap(u32 top_k, ScoreType *score, ID *id) : score_(score), id_(id), top_k_(top_k) {
        score_ -= 1;
        id_ -= 1;
    }

    [[nodiscard]] u32 GetResultSize() const { return size_; }

    [[nodiscard]] ScoreType GetScoreThreshold() const { return size_ < top_k_ ? 0 : score_[1]; }

    // return true if the threshold is updated
    bool AddResult(ScoreType d, ID i) {
        if (size_ < top_k_) {
            ++size_;
            score_[size_] = d;
            id_[size_] = i;
            if (size_ == top_k_) {
                for (u32 index = size_ / 2; index > 0; --index) {
                    HeapifyDown(size_, index);
                }
                return true;
            }
        } else if (Compare::Less(score_[1], d, id_[1], i)) {
            score_[1] = d;
            id_[1] = i;
            HeapifyDown(size_, 1);
            return true;
        }
        return false;
    }

    void Sort() {
        u32 size = size_;
        if (size < top_k_) {
            std::fill_n(score_ + size + 1, top_k_ - size, Compare::MinValue());
            for (u32 index = size / 2; index > 0; --index) {
                HeapifyDown(size, index);
            }
        }
        while (size > 1) {
            std::swap(score_[size], score_[1]);
            std::swap(id_[size], id_[1]);
            --size;
            HeapifyDown(size, 1);
        }
    }
};

} // namespace infinity