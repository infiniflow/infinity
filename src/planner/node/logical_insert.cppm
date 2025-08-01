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

export module infinity_core:logical_insert;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;

import :base_expression;
import :meta_info;
import internal_types;
import data_type;

namespace infinity {

export class LogicalInsert : public LogicalNode {
public:
    explicit inline LogicalInsert(u64 node_id, SharedPtr<TableInfo> table_info, u64 table_index, Vector<Vector<SharedPtr<BaseExpression>>> value_list)
        : LogicalNode(node_id, LogicalNodeType::kInsert), table_info_(std::move(table_info)), value_list_(std::move(value_list)),
          table_index_(table_index) {};

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalInsert"; }

    inline void set_value_list(const Vector<Vector<SharedPtr<BaseExpression>>> &value_list) { value_list_ = value_list; }

    inline SharedPtr<TableInfo>& table_info() { return table_info_; }
    inline const SharedPtr<TableInfo>& table_info() const { return table_info_; }

    inline Vector<Vector<SharedPtr<BaseExpression>>> &value_list() { return value_list_; }

    [[nodiscard]] inline const Vector<Vector<SharedPtr<BaseExpression>>> &value_list() const { return value_list_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    // Check if this insert uses a SELECT source (has child node) or direct values
    [[nodiscard]] inline bool has_select_source() const { return left_node_.get() != nullptr; }

public:
    static bool NeedCastInInsert(const DataType &from, const DataType &to) {
        if (from.type() == to.type()) {
            switch (from.type()) {
                // Embedding
                // Bitmap
                // Decimal
                default:
                    break;
            }
        }
        return true;
    }

private:
    SharedPtr<TableInfo> table_info_{};
    Vector<Vector<SharedPtr<BaseExpression>>> value_list_{};
    u64 table_index_{};
};

} // namespace infinity
