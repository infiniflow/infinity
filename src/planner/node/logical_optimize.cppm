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

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_optimize;

namespace infinity {

export class LogicalOptimize : public LogicalNode {
public:
    explicit LogicalOptimize(u64 node_id, String schema_name, String object_name)
        : LogicalNode(node_id, LogicalNodeType::kOptimize), schema_name_(Move(schema_name)), object_name_(Move(object_name)) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalOptimize"; }

    [[nodiscard]] OptimizeType optimize_type() const { return optimize_type_; }

    [[nodiscard]] inline const String &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const String &object_name() const { return object_name_; }

private:
    OptimizeType optimize_type_{OptimizeType::kIRS};
    String schema_name_;
    String object_name_; // It could be table/collection/view name
};

} // namespace infinity
