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

#include <functional>

import stl;

export module column_binding;

namespace infinity {

export struct ColumnBinding {

    ColumnBinding() = default;

    explicit ColumnBinding(SizeT tbl_idx, SizeT col_idx) : table_idx(tbl_idx), column_idx(col_idx) {}

    friend auto operator<=>(const ColumnBinding &lhs, const ColumnBinding &rhs) = default;

    SizeT table_idx{};
    SizeT column_idx{};
};

} // namespace infinity

namespace std {

template <>
class hash<infinity::ColumnBinding> {
public:
    std::size_t operator()(const infinity::ColumnBinding &rhs) const {
        return hash<std::size_t>().operator()(rhs.table_idx) ^ hash<std::size_t>().operator()(rhs.column_idx);
    }
};

} // namespace std
