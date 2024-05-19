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

#include <sstream>

module logical_compact_index;

import stl;
import column_binding;
import data_type;
import table_entry;
import logical_type;

namespace infinity {

Vector<ColumnBinding> LogicalCompactIndex::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalCompactIndex::GetOutputNames() const {
    auto result = MakeShared<Vector<String>>();
    result->push_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalCompactIndex::GetOutputTypes() const {
    auto result = MakeShared<Vector<SharedPtr<DataType>>>();
    result->push_back(MakeShared<DataType>(LogicalType::kInteger));
    return result;
}

String LogicalCompactIndex::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "COMPACT INDEX";
    auto *table_entry = base_table_ref_->table_entry_ptr_;
    ss << *table_entry->GetDBName() << "." << *table_entry->GetTableName();
    space += arrow_str.size();
    return ss.str();
}

} // namespace infinity
