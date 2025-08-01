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

module infinity_core:logical_alter.impl;

import :logical_alter;

import logical_type;
import :stl;
import data_type;
import :column_binding;
import :logical_alter;

namespace infinity {

Vector<ColumnBinding> LogicalAlter::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalAlter::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    result->emplace_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalAlter::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
    result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    return result_type;
}

String LogicalRenameTable::ToString(i64 &space) const {
    //
    return "";
}

String LogicalAddColumns::ToString(i64 &space) const {
    //
    return "";
}

String LogicalDropColumns::ToString(i64 &space) const {
    //
    return "";
}

} // namespace infinity