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

export module infinity_core:logical_view_scan;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import :view;

import internal_types;
import data_type;

namespace infinity {

export class LogicalViewScan : public LogicalNode {

public:
    explicit LogicalViewScan(u64 node_id, SharedPtr<View> view_ptr)
        : LogicalNode(node_id, LogicalNodeType::kViewScan), view_ptr_(std::move(view_ptr)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] SharedPtr<View> view_ptr() const { return view_ptr_; }

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalViewScan"; }

private:
    SharedPtr<View> view_ptr_{nullptr};
};

} // namespace infinity
