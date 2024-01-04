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

import stl;
import parser;
import logical_node;
import logical_node_type;
import column_binding;
import base_table_ref;
import column_binding;
import logical_node_type;

module logical_update;

namespace infinity {

Vector<ColumnBinding> LogicalUpdate::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalUpdate::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    result->emplace_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalUpdate::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
    result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    return result_type;
}

String LogicalUpdate::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "UPDATE ";
    ss << *table_entry_ptr_->GetDBName() << "." << *table_entry_ptr_->GetTableName();
    ss << " SET";
    for (SizeT i = 0; i < update_columns_.size(); i++) {
        if (i > 0)
            ss << ", ";
        ss << " ";
        ss << table_entry_ptr_->GetColumnDefByID(update_columns_[i].first)->name_;
        ss << " = " << update_columns_[i].second->Name();
    }
    return ss.str();
}

} // namespace infinity
