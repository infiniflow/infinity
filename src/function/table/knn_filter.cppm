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


import stl;
import hnsw_common;
import bitmask;
import catalog;

export module knn_filter;

namespace infinity {

export template <typename LabelType>
class BitmaskFilter : public FilterBase<LabelType> {
public:
    BitmaskFilter(LabelGetter<LabelType> &&label_getter, const Bitmask &bitmask)
        : FilterBase<LabelType>(std::move(label_getter)), bitmask_(bitmask) {}

    bool operator()(const VertexType &vertex_i) const final { return bitmask_.IsTrue(this->label_getter_(vertex_i)); }

private:
    const Bitmask &bitmask_;
};

} // namespace infinity
