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

export module physical_match_tensor_scan;

import stl;
import query_context;
import operator_state;
import physical_operator;
import table_entry;
import match_tensor_expression;
import base_table_ref;
import data_type;
import common_query_filter;
import physical_filter_scan_base;
import global_block_id;

namespace infinity {
struct LoadMeta;
struct BlockIndex;
struct BlockColumnEntry;
class SegmentIndexEntry;

export class PhysicalMatchTensorScan final : public PhysicalFilterScanBase {
public:
    explicit PhysicalMatchTensorScan(u64 id,
                                     u64 table_index,
                                     SharedPtr<BaseTableRef> base_table_ref,
                                     SharedPtr<MatchTensorExpression> match_tensor_expression,
                                     const SharedPtr<CommonQueryFilter> &common_query_filter,
                                     u32 topn,
                                     SharedPtr<Vector<LoadMeta>> load_metas);

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override;

    ColumnID SearchColumnID() const;

    void CheckColumn();

    void PlanWithIndex(QueryContext *query_context);

    Vector<SharedPtr<Vector<GlobalBlockID>>> PlanBlockEntries(i64 parallel_count) const override;

    SizeT TaskletCount() override;

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    [[nodiscard]] inline String TableAlias() const { return base_table_ref_->alias_; }

    [[nodiscard]] inline TableEntry *table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline MatchTensorExpression *match_tensor_expr() const { return match_tensor_expr_.get(); }

    [[nodiscard]] inline const CommonQueryFilter *common_query_filter() const { return common_query_filter_.get(); }

    [[nodiscard]] inline u32 GetTopN() const { return topn_; }

private:
    u64 table_index_ = 0;
    SharedPtr<MatchTensorExpression> match_tensor_expr_;

    // extra options from match_tensor_expr_
    u32 topn_ = 0;

    // column to search
    ColumnID search_column_id_ = 0;

    Vector<SegmentIndexEntry *> index_entries_;
    Vector<BlockColumnEntry *> block_column_entries_;
    mutable atomic_u32 task_executed_ = 0;

    void ExecuteInner(QueryContext *query_context, MatchTensorScanOperatorState *operator_state) const;
};

struct MatchTensorRerankDoc;
class BufferManager;
export void CalculateFusionMatchTensorRerankerScores(Vector<MatchTensorRerankDoc> &rerank_docs,
                                                     BufferManager *buffer_mgr,
                                                     const DataType *column_data_type,
                                                     ColumnID column_id,
                                                     const BlockIndex *block_index,
                                                     const MatchTensorExpression &match_tensor_expr);

} // namespace infinity
