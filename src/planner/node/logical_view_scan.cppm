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
    explicit LogicalViewScan(u64 node_id, std::shared_ptr<View> view_ptr)
        : LogicalNode(node_id, LogicalNodeType::kViewScan), view_ptr_(std::move(view_ptr)) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] std::shared_ptr<View> view_ptr() const { return view_ptr_; }

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalViewScan"; }

private:
    std::shared_ptr<View> view_ptr_{nullptr};
};

} // namespace infinity
