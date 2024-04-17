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
import table_index_entry;
import fast_rough_filter;
import secondary_index_scan_execute_expression;
import bitmask;

namespace infinity {

export class PhysicalMatch final : public PhysicalOperator {
public:
    explicit PhysicalMatch(u64 id,
                           SharedPtr<BaseTableRef> base_table_ref,
                           SharedPtr<MatchExpression> match_expr,
                           bool have_filter,
                           UniquePtr<FastRoughFilterEvaluator> &&fast_rough_filter_evaluator,
                           const SharedPtr<BaseExpression> &filter_leftover,
                           const SharedPtr<BaseExpression> &secondary_index_filter_qualified,
                           HashMap<ColumnID, TableIndexEntry *> &&secondary_index_column_index_map,
                           Vector<FilterExecuteElem> &&filter_execute_command,
                           u64 match_table_index,
                           SharedPtr<Vector<LoadMeta>> load_metas);

    ~PhysicalMatch() override;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    SizeT TaskletCount() override {
        UnrecoverableError("Not implement: TaskletCount not Implement");
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

    [[nodiscard]] inline bool have_filter() const { return have_filter_; }

    [[nodiscard]] inline const BaseExpression *filter_leftover_expr() const { return filter_leftover_.get(); }

    [[nodiscard]] inline const BaseExpression *filter_secondary_index_expr() const { return secondary_index_filter_qualified_.get(); }

private:
    u64 table_index_ = 0;
    SharedPtr<BaseTableRef> base_table_ref_;
    SharedPtr<MatchExpression> match_expr_;

    // for filter
    bool have_filter_ = false;
    UniquePtr<FastRoughFilterEvaluator> fast_rough_filter_evaluator_;
    // TODO: filter_leftover_ is not handled now
    SharedPtr<BaseExpression> filter_leftover_;
    SharedPtr<BaseExpression> secondary_index_filter_qualified_;
    HashMap<ColumnID, TableIndexEntry *> secondary_index_column_index_map_;
    // secondary index filter execute expression
    Vector<FilterExecuteElem> filter_execute_command_;
    // filter result, form an iterator
    Map<SegmentID, std::variant<Vector<u32>, Bitmask>> filter_result_;
    SizeT filter_result_count_ = 0;

    bool ExecuteInner(QueryContext *query_context, OperatorState *operator_state);
    bool ExecuteInnerHomebrewed(QueryContext *query_context, OperatorState *operator_state);
};

} // namespace infinity
