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

module infinity_core:logical_explain.impl;

import :logical_explain;
import :stl;
import :column_binding;
import :base_expression;

import internal_types;
import data_type;

namespace infinity {

Vector<ColumnBinding> LogicalExplain::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalExplain::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalExplain::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalExplain::ToString(i64 &space) const { return "LogicalExplain"; }

} // namespace infinity