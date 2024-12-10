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

export module logical_limit;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import data_type;
import base_expression;
import internal_types;

namespace infinity {

export class LogicalLimit : public LogicalNode {
public:
    inline explicit LogicalLimit(u64 node_id,
                                 SharedPtr<BaseExpression> limit_expression,
                                 SharedPtr<BaseExpression> offset_expression,
                                 bool total_hits_count_flag)
        : LogicalNode(node_id, LogicalNodeType::kLimit), limit_expression_(std::move(limit_expression)),
          offset_expression_(std::move(offset_expression)), total_hits_count_flag_(total_hits_count_flag) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalLimit"; }

    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseExpression> offset_expression_{};

    bool total_hits_count_flag_{false};
};

} // namespace infinity
