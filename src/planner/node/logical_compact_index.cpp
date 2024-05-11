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

module logical_compact_index;

import stl;
import column_binding;
import data_type;

namespace infinity {

Vector<ColumnBinding> LogicalCompactIndex::GetColumnBindings() const {
    //
    return {};
}

SharedPtr<Vector<String>> LogicalCompactIndex::GetOutputNames() const {
    //
    return nullptr;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalCompactIndex::GetOutputTypes() const {
    //
    return nullptr;
}

String LogicalCompactIndex::ToString(i64 &space) const {
    //
    return {};
}

} // namespace infinity
