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

export module filter_expression_push_down;

import stl;
import query_context;
import base_expression;
import base_table_ref;
import index_base;
import table_index_entry;
import secondary_index_scan_execute_expression;
import fast_rough_filter;

namespace infinity {

// TODO: equivalent expression rewrite optimization

// TODO: now do not support "not" expression in index scan

export struct IndexScanFilterExpressionPushDownResult {
    IndexScanFilterExpressionPushDownResult(HashMap<ColumnID, TableIndexEntry *> &&column_index_map,
                                            SharedPtr<BaseExpression> &&index_filter_qualified,
                                            SharedPtr<BaseExpression> &&extra_leftover_filter,
                                            Vector<FilterExecuteElem> &&filter_execute_command)
        : column_index_map_(std::move(column_index_map)), index_filter_qualified_(std::move(index_filter_qualified)),
          extra_leftover_filter_(std::move(extra_leftover_filter)), filter_execute_command_(std::move(filter_execute_command)) {}
    HashMap<ColumnID, TableIndexEntry *> column_index_map_;
    SharedPtr<BaseExpression> index_filter_qualified_;
    SharedPtr<BaseExpression> extra_leftover_filter_;
    Vector<FilterExecuteElem> filter_execute_command_;
};

export class FilterExpressionPushDown {
public:
    static IndexScanFilterExpressionPushDownResult
    PushDownToIndexScan(QueryContext *query_context, const BaseTableRef &base_table_ref, const SharedPtr<BaseExpression> &expression);

    static UniquePtr<FastRoughFilterEvaluator> PushDownToFastRoughFilter(SharedPtr<BaseExpression> &expression);
};

} // namespace infinity