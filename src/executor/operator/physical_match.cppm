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

export module physical_match;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import table_entry;
import base_expression;
import match_expression;
import base_table_ref;
import load_meta;
import infinity_exception;
import internal_types;
import data_type;
import common_query_filter;
import logger;
import column_index_reader;
import query_node;

namespace infinity {

export class PhysicalMatch final : public PhysicalOperator {
public:
    explicit PhysicalMatch(u64 id,
                           SharedPtr<BaseTableRef> base_table_ref,
                           SharedPtr<MatchExpression> match_expr,
                           IndexReader index_reader,
                           UniquePtr<QueryNode>&& query_tree,
                           float begin_threshold,
                           const SharedPtr<CommonQueryFilter> &common_query_filter,
                           u64 match_table_index,
                           SharedPtr<Vector<LoadMeta>> load_metas);

    ~PhysicalMatch() override;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return 0;
    }

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    String ToString(i64 &space) const;

    [[nodiscard]] inline String TableAlias() const { return base_table_ref_->alias_; }

    [[nodiscard]] inline TableEntry *table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline MatchExpression *match_expr() const { return match_expr_.get(); }

    [[nodiscard]] inline const CommonQueryFilter *common_query_filter() const { return common_query_filter_.get(); }

private:
    u64 table_index_ = 0;
    SharedPtr<BaseTableRef> base_table_ref_;
    SharedPtr<MatchExpression> match_expr_;
    IndexReader index_reader_;
    UniquePtr<QueryNode> query_tree_;
    float begin_threshold_;

    // for filter
    SharedPtr<CommonQueryFilter> common_query_filter_;

    bool ExecuteInner(QueryContext *query_context, OperatorState *operator_state);
    bool ExecuteInnerHomebrewed(QueryContext *query_context, OperatorState *operator_state);
};

} // namespace infinity
