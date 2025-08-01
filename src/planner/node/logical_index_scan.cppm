//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

export module infinity_core:logical_index_scan;

import :stl;
import :logical_node_type;
import :logical_node;
import :column_binding;
import :base_table_ref;
import :base_expression;
import :default_values;
import :filter_expression_push_down;
import data_type;
import :meta_info;
import :fast_rough_filter;

namespace infinity {

export class LogicalIndexScan : public LogicalNode {
public:
    explicit LogicalIndexScan(u64 node_id,
                              SharedPtr<BaseTableRef> &&base_table_ref,
                              SharedPtr<BaseExpression> &&index_filter,
                              UniquePtr<IndexFilterEvaluator> &&index_filter_evaluator,
                              UniquePtr<FastRoughFilterEvaluator> &&fast_rough_filter_evaluator,
                              bool add_row_id = true);

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] TableInfo *table_info() const;

    [[nodiscard]] String TableAlias() const;

    [[nodiscard]] u64 TableIndex() const;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalIndexScan"; }

    SharedPtr<BaseTableRef> base_table_ref_{};

    SharedPtr<BaseExpression> index_filter_;
    UniquePtr<IndexFilterEvaluator> index_filter_evaluator_;

    UniquePtr<FastRoughFilterEvaluator> fast_rough_filter_evaluator_;

    bool add_row_id_;
};

} // namespace infinity
