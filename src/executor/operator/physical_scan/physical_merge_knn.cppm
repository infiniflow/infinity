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

export module physical_merge_knn;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import data_table;
import base_table_ref;
import load_meta;
import knn_expression;
import infinity_exception;
import internal_types;
import data_type;
import logger;
import physical_scan_base;

namespace infinity {

export class PhysicalMergeKnn final : public PhysicalScanBase {
public:
    explicit PhysicalMergeKnn(u64 id,
                              SharedPtr<BaseTableRef> table_ref,
                              UniquePtr<PhysicalOperator> left,
                              SharedPtr<Vector<String>> output_names,
                              SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                              SharedPtr<KnnExpression> knn_expr,
                              SharedPtr<BaseExpression> filter_expression,
                              u64 knn_table_index,
                              SharedPtr<Vector<LoadMeta>> load_metas,
                              bool cache_result)
        : PhysicalScanBase(id, PhysicalOperatorType::kMergeKnn, std::move(left), nullptr, knn_table_index, table_ref, load_metas, cache_result),
          output_names_(std::move(output_names)), output_types_(std::move(output_types)), knn_expression_(std::move(knn_expr)),
          filter_expression_(std::move(filter_expression)) {}

    ~PhysicalMergeKnn() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        UnrecoverableError(error_message);
        return 0;
    }

    inline bool IsKnnMinHeap() const { return knn_expression_->IsKnnMinHeap(); }

private:
    template <typename T, template <typename, typename> typename C>
    void ExecuteInner(QueryContext *query_context, MergeKnnOperatorState *operator_state);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

public:
    SharedPtr<KnnExpression> knn_expression_{};
    SharedPtr<BaseExpression> filter_expression_{};
};

} // namespace infinity
