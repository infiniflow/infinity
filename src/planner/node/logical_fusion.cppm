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

export module logical_fusion;

namespace infinity {

class FusionExpression;
class MatchExpression;
class BaseTableRef;
class TableCollectionEntry;

export class LogicalFusion : public LogicalNode {
public:
    explicit LogicalFusion(u64 node_id,
                           SharedPtr<FusionExpression> fusion_expr,
                           const Vector<ColumnBinding> &column_bindings,
                           SharedPtr<Vector<String>> output_names,
                           SharedPtr<Vector<SharedPtr<DataType>>> output_types);

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalFusion"; }

    SharedPtr<FusionExpression> fusion_expr_{};

private:
    Vector<ColumnBinding> column_bindings_{};
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
