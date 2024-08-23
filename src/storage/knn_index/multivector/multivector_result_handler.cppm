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

export module multivector_result_handler;

import stl;
import infinity_exception;
import internal_types;
import third_party;

namespace infinity {

// true: 1 sort after 2
inline bool CompareDistanceIdPair(const auto d1, const auto d2, const auto id1, const auto id2) { return (d1 > d2) || ((d1 == d2) && (id1 > id2)); }

template <typename RawDataT, typename RawLabelT, std::unsigned_integral InnerIndexT>
    requires(sizeof(InnerIndexT) <= sizeof(u32))
struct SegmentTreeForTopnDistance {
    RawDataT *raw_data_ = nullptr;
    RawLabelT *raw_label_ = nullptr;
    RawDataT max_raw_data_ = std::numeric_limits<RawDataT>::max();
    RawLabelT max_raw_label_ = {};
    const u32 size_ = 0;
    UniquePtr<InnerIndexT[]> inner_index_ = MakeUniqueForOverwrite<InnerIndexT[]>(size_ << 1);

    explicit SegmentTreeForTopnDistance(RawDataT *raw_data, RawLabelT *raw_label, const u32 size)
        : raw_data_(raw_data), raw_label_(raw_label), size_(size) {}

    [[nodiscard]] InnerIndexT index_max_v(const InnerIndexT lhs, const InnerIndexT rhs) const {
        return CompareDistanceIdPair(raw_data_[lhs], raw_data_[rhs], raw_label_[lhs], raw_label_[rhs]) ? lhs : rhs;
    }

    void Build() {
        // init inner_index_ for leaf nodes
        std::iota(inner_index_.get() + size_, inner_index_.get() + (size_ << 1), InnerIndexT{});
        // init inner_index_ for non-leaf nodes
        for (u32 i = size_ - 1; i > 0; --i) {
            inner_index_[i] = index_max_v(inner_index_[i << 1], inner_index_[(i << 1) + 1]);
        }
        max_raw_data_ = raw_data_[inner_index_[1]];
        max_raw_label_ = raw_label_[inner_index_[1]];
    }

    void Update(InnerIndexT index) {
        for (u32 i = ((size_ + index) >> 1); i > 0; i >>= 1) {
            inner_index_[i] = index_max_v(inner_index_[i << 1], inner_index_[(i << 1) + 1]);
        }
        max_raw_data_ = raw_data_[inner_index_[1]];
        max_raw_label_ = raw_label_[inner_index_[1]];
    }

    [[nodiscard]] auto GetDistanceBar() const { return max_raw_data_; }
};

// now only support one query
export template <typename DistanceType, typename LabelType, typename InnerTopnIndexType>
class MultiVectorResultHandler {
    const SizeT top_k_ = 0;
    FlatHashMap<LabelType, InnerTopnIndexType> label_index_map_;
    SegmentTreeForTopnDistance<DistanceType, LabelType, InnerTopnIndexType> segment_tree_for_top_n_distance_;

public:
    explicit MultiVectorResultHandler(const SizeT top_k, DistanceType *distance_ptr, LabelType *label_ptr)
        : top_k_(top_k), segment_tree_for_top_n_distance_(distance_ptr, label_ptr, top_k) {}

    // do nothing
    void Begin() const {}

    // return the biggest distance in map
    // call AddResult if distance <= GetDistanceBar()
    [[nodiscard]] auto GetDistanceBar() const { return segment_tree_for_top_n_distance_.GetDistanceBar(); }

    void AddResult(DistanceType d, LabelType l) {
        if (const auto it = label_index_map_.find(l); it != label_index_map_.end()) {
            // already in result set
            const auto inner_index = it->second;
            if (auto &old_distance = segment_tree_for_top_n_distance_.raw_data_[inner_index]; d < old_distance) {
                old_distance = d;
                if (label_index_map_.size() == top_k_) {
                    segment_tree_for_top_n_distance_.Update(inner_index);
                }
            }
        } else {
            const auto current_size = label_index_map_.size();
            if (current_size == top_k_) {
                // need replace?
                if (CompareDistanceIdPair(segment_tree_for_top_n_distance_.max_raw_data_, d, segment_tree_for_top_n_distance_.max_raw_label_, l)) {
                    // replace
                    label_index_map_.erase(segment_tree_for_top_n_distance_.max_raw_label_);
                    const auto inner_index = segment_tree_for_top_n_distance_.inner_index_[1];
                    label_index_map_.emplace(l, inner_index);
                    segment_tree_for_top_n_distance_.raw_label_[inner_index] = l;
                    segment_tree_for_top_n_distance_.raw_data_[inner_index] = d;
                    segment_tree_for_top_n_distance_.Update(inner_index);
                }
            } else {
                // append, may need build
                label_index_map_.emplace(l, current_size);
                segment_tree_for_top_n_distance_.raw_label_[current_size] = l;
                segment_tree_for_top_n_distance_.raw_data_[current_size] = d;
                if (current_size + 1 == top_k_) {
                    segment_tree_for_top_n_distance_.Build();
                }
            }
        }
    }

    // do nothing
    void EndWithoutSort() const {}
};

} // namespace infinity
