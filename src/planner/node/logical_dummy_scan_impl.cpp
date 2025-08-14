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

module infinity_core:logical_dummy_scan.impl;

import :logical_dummy_scan;
import :stl;
import :column_binding;
import :logical_dummy_scan;

import std;

import data_type;
// import internal_types;

namespace infinity {

Vector<ColumnBinding> LogicalDummyScan::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalDummyScan::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalDummyScan::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalDummyScan::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << "-> "
       << "DummyScan: " << this->table_alias_ << "(" << this->table_index_ << ")";
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
