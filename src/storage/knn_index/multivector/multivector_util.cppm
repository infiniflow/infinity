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

export module multivector_util;

import stl;
import third_party;

namespace infinity {

export template <typename ElementT>
class MultiVectorRef {
    Span<const char> raw_data_ = {};
    SizeT embedding_num_ = 0;

public:
    using ElementType = ElementT;
    MultiVectorRef &operator=(const Pair<Span<const char>, SizeT> &data) {
        raw_data_ = data.first;
        embedding_num_ = data.second;
        return *this;
    }
    [[nodiscard]] Span<const char> raw_data() const { return raw_data_; }
    [[nodiscard]] SizeT embedding_num() const { return embedding_num_; }
};

} // namespace infinity