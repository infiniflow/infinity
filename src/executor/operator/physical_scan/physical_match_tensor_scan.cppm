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
import logical_match_tensor_scan;
import internal_types;

namespace infinity {
struct LoadMeta;
struct BlockIndex;
struct BlockColumnEntry;
struct SegmentIndexEntry;

export class PhysicalMatchTensorScan final : public PhysicalFilterScanBase {
public:
    explicit PhysicalMatchTensorScan(u64 id,
                                     u64 table_index,
                                     SharedPtr<BaseTableRef> base_table_ref,
                                     SharedPtr<MatchTensorExpression> match_tensor_expression,
                                     const SharedPtr<CommonQueryFilter> &common_query_filter,
                                     u32 topn,
                                     const SharedPtr<MatchTensorScanIndexOptions> &index_options,
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

    [[nodiscard]] inline String TableAlias() const { return base_table_ref_->alias_; }

    [[nodiscard]] inline TableEntry *table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

    [[nodiscard]] inline const SharedPtr<MatchTensorExpression> &match_tensor_expr() const { return src_match_tensor_expr_; }

    [[nodiscard]] inline const CommonQueryFilter *common_query_filter() const { return common_query_filter_.get(); }

    [[nodiscard]] inline u32 GetTopN() const { return topn_; }

    const SharedPtr<MatchTensorScanIndexOptions> &index_options() const { return index_options_; }

private:
    SharedPtr<MatchTensorExpression> src_match_tensor_expr_;

    // real MatchTensorExpression used in calculation
    SharedPtr<void> calc_match_tensor_aligned_holder_;
    UniquePtr<MatchTensorExpression> calc_match_tensor_expr_holder_;
    MatchTensorExpression *calc_match_tensor_expr_ = nullptr;

    // extra options from match_tensor_expr_
    u32 topn_ = 0;
    SharedPtr<MatchTensorScanIndexOptions> index_options_;

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
                                                     MatchTensorExpression &src_match_tensor_expr);

// u8, i8, i16, i32 -> i32
// i64 -> i64
// f32, f64, float16, bfloat16 -> f32
export void *GetAlignedCast(char *src_embedding_ptr, u32 dim, EmbeddingDataType src_embedding_data_type, EmbeddingDataType new_embedding_data_type);

} // namespace infinity
