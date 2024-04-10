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

#include <limits>
#include <utility>

export module hnsw_common;

import stl;
import file_system;
import infinity_exception;

namespace infinity {

export struct HnswConfig {
    SizeT lvq_buffer_size_;
};

export constexpr SizeT AlignTo(SizeT a, SizeT b) { return (a + b - 1) / b * b; }

export using MeanType = double;
export using VertexType = i32;
export using VertexListSize = i32;
export using LayerSize = i32;

export template <typename Iterator, typename DataType, typename LabelType>
concept DataIteratorConcept = requires(Iterator iter) {
    { iter.Next() } -> std::same_as<Optional<Pair<DataType, LabelType>>>;
};

export template <typename DataType, typename LabelType>
class DenseVectorIter {
    const DataType *ptr_;
    const SizeT dim_;
    const DataType *const ptr_end_;
    LabelType label_;

public:
    DenseVectorIter(const DataType *ptr, SizeT dim, SizeT vec_num, LabelType offset = 0)
        : ptr_(ptr), dim_(dim), ptr_end_(ptr_ + dim * vec_num), label_(offset) {}

    Optional<Pair<const DataType *, LabelType>> Next() {
        auto ret = ptr_;
        if (ret == ptr_end_) {
            return None;
        }
        ptr_ += dim_;
        return std::make_pair(ret, label_++);
    }
};

export template <typename LabelType>
class FilterBase {
public:
    virtual bool operator()(const LabelType &vertex_i) const = 0;
};

export template <typename Filter, typename LabelType>
concept FilterConcept = requires(LabelType label) { std::is_same_v<Filter, NoneType> || std::is_base_of_v<FilterBase<LabelType>, Filter>; };

export struct HnswInsertConfig {
    bool optimize_;
};

export constexpr HnswInsertConfig kDefaultHnswInsertConfig = {
    .optimize_ = false,
};

} // namespace infinity