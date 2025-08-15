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

export module infinity_core:logical_create_schema;

import :logical_node_type;
import :column_binding;
import :logical_node;

import data_type;
import internal_types;
import extra_ddl_info;

namespace infinity {

export class LogicalCreateSchema : public LogicalNode {
public:
    static inline std::shared_ptr<LogicalCreateSchema>
    Make(u64 node_id, std::shared_ptr<std::string> schema_name, ConflictType conflict_type, std::shared_ptr<std::string> comment) {
        return std::make_shared<LogicalCreateSchema>(node_id, std::move(schema_name), conflict_type, std::move(comment));
    }

public:
    LogicalCreateSchema(u64 node_id, const std::shared_ptr<std::string> schema_name, ConflictType conflict_type, std::shared_ptr<std::string> comment)
        : LogicalNode(node_id, LogicalNodeType::kCreateSchema), schema_name_(std::move(schema_name)), conflict_type_(conflict_type),
          comment_(std::move(comment)) {}

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalCreateSchema"; }

    [[nodiscard]] inline std::shared_ptr<std::string> schema_name() const { return schema_name_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

    [[nodiscard]] inline std::shared_ptr<std::string> comment() const { return comment_; }

private:
    std::shared_ptr<std::string> schema_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
    std::shared_ptr<std::string> comment_{};
};

} // namespace infinity
