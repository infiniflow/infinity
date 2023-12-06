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
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import data_table;
import base_table_ref;
import load_meta;

export module physical_merge_knn;

namespace infinity {

export class PhysicalMergeKnn final : public PhysicalOperator {
public:
    explicit PhysicalMergeKnn(u64 id,
                              SharedPtr<BaseTableRef> table_ref,
                              UniquePtr<PhysicalOperator> left,
                              SharedPtr<Vector<String>> output_names,
                              SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                              Vector<SharedPtr<BaseExpression>> knn_exprs,
                              SharedPtr<BaseExpression> limit_expr,
                              OrderType order_by_type,
                              u64 knn_table_index,
                              SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kMergeKnn, Move(left), nullptr, id, load_metas), table_ref_(table_ref), output_names_(Move(output_names)),
          output_types_(Move(output_types)), knn_expressions_(Move(knn_exprs)), limit_expression_(Move(limit_expr)), order_by_type_(order_by_type),
          knn_table_index_(knn_table_index) {}

    ~PhysicalMergeKnn() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline u64 knn_table_index() const { return knn_table_index_; }

private:
    template <typename T, template <typename, typename> typename C>
    void ExecuteInner(QueryContext *query_context, MergeKnnOperatorState *operator_state);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    u64 knn_table_index_{};

public:
    Vector<SharedPtr<BaseExpression>> knn_expressions_{};
    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseTableRef> table_ref_{};

private:
    OrderType order_by_type_{OrderType::kAsc};
};

} // namespace infinity
