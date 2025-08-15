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

export module infinity_core:logical_create_view;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;

import data_type;
import internal_types;
import create_view_info;

namespace infinity {

export class LogicalCreateView : public LogicalNode {
public:
    static inline std::shared_ptr<LogicalCreateView> Make(u64 node_id,
                                                    const std::shared_ptr<std::vector<std::string>> &names_ptr,
                                                    const std::shared_ptr<std::vector<std::shared_ptr<DataType>>> &types_ptr,
                                                    const std::shared_ptr<CreateViewInfo> &create_view_info) {
        return std::make_shared<LogicalCreateView>(node_id, names_ptr, types_ptr, create_view_info);
    }

public:
    LogicalCreateView(u64 node_id,
                      std::shared_ptr<std::vector<std::string>> names_ptr,
                      std::shared_ptr<std::vector<std::shared_ptr<DataType>>> types_ptr,
                      std::shared_ptr<CreateViewInfo> create_view_info)
        : LogicalNode(node_id, LogicalNodeType::kCreateView), names_ptr_(std::move(names_ptr)), types_ptr_(std::move(types_ptr)),
          create_view_info_(std::move(create_view_info)) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalCreateView"; }

    [[nodiscard]] inline const std::shared_ptr<std::vector<std::string>> &names_ptr() const { return names_ptr_; };

    [[nodiscard]] inline const std::shared_ptr<std::vector<std::shared_ptr<DataType>>> &types_ptr() const { return types_ptr_; }

    [[nodiscard]] const std::shared_ptr<CreateViewInfo> create_view_info() const;

private:
    std::shared_ptr<std::vector<std::string>> names_ptr_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> types_ptr_{};
    std::shared_ptr<CreateViewInfo> create_view_info_{nullptr};
};

} // namespace infinity
