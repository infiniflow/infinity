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
    static inline SharedPtr<LogicalCreateView> Make(u64 node_id,
                                                    const SharedPtr<Vector<String>> &names_ptr,
                                                    const SharedPtr<Vector<SharedPtr<DataType>>> &types_ptr,
                                                    const SharedPtr<CreateViewInfo> &create_view_info) {
        return MakeShared<LogicalCreateView>(node_id, names_ptr, types_ptr, create_view_info);
    }

public:
    LogicalCreateView(u64 node_id,
                      SharedPtr<Vector<String>> names_ptr,
                      SharedPtr<Vector<SharedPtr<DataType>>> types_ptr,
                      SharedPtr<CreateViewInfo> create_view_info)
        : LogicalNode(node_id, LogicalNodeType::kCreateView), names_ptr_(std::move(names_ptr)), types_ptr_(std::move(types_ptr)),
          create_view_info_(std::move(create_view_info)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCreateView"; }

    [[nodiscard]] inline const SharedPtr<Vector<String>> &names_ptr() const { return names_ptr_; };

    [[nodiscard]] inline const SharedPtr<Vector<SharedPtr<DataType>>> &types_ptr() const { return types_ptr_; }

    [[nodiscard]] const SharedPtr<CreateViewInfo> create_view_info() const;

private:
    SharedPtr<Vector<String>> names_ptr_{};
    SharedPtr<Vector<SharedPtr<DataType>>> types_ptr_{};
    SharedPtr<CreateViewInfo> create_view_info_{nullptr};
};

} // namespace infinity
