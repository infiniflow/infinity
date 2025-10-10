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

export module infinity_core:physical_match_tensor_scan;

import :query_context;
import :operator_state;
import :physical_operator;
import :match_tensor_expression;
import :base_table_ref;
import :common_query_filter;
import :physical_filter_scan_base;
import :global_block_id;
import :logical_match_tensor_scan;
import :meta_info;

import data_type;
import internal_types;

namespace infinity {
struct LoadMeta;
struct BlockIndex;

class BlockMeta;
class TableIndexMeta;
class SegmentIndexMeta;

export class PhysicalMatchTensorScan final : public PhysicalFilterScanBase {
public:
    explicit PhysicalMatchTensorScan(u64 id,
                                     u64 table_index,
                                     std::shared_ptr<BaseTableRef> base_table_ref,
                                     std::shared_ptr<MatchTensorExpression> match_tensor_expression,
                                     const std::shared_ptr<CommonQueryFilter> &common_query_filter,
                                     u32 topn,
                                     std::optional<f32> knn_threshold,
                                     const std::shared_ptr<MatchTensorScanIndexOptions> &index_options,
                                     std::shared_ptr<std::vector<LoadMeta>> load_metas);

    ~PhysicalMatchTensorScan() override;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const override;

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const override;

    ColumnID SearchColumnID() const;

    void CheckColumn();

    void PlanWithIndex(QueryContext *query_context);

    std::vector<std::shared_ptr<std::vector<GlobalBlockID>>> PlanBlockEntries(i64 parallel_count) const override;

    size_t TaskletCount() override;

    [[nodiscard]] inline std::string TableAlias() const { return base_table_ref_->alias_; }

    //    [[nodiscard]] inline TableInfo *table_info() const { return base_table_ref_->table_info_.get(); }

    [[nodiscard]] inline const std::shared_ptr<MatchTensorExpression> &match_tensor_expr() const { return src_match_tensor_expr_; }

    [[nodiscard]] inline const CommonQueryFilter *common_query_filter() const { return common_query_filter_.get(); }

    [[nodiscard]] inline u32 GetTopN() const { return topn_; }

    [[nodiscard]] inline std::optional<f32> GetKnnThreshold() const { return knn_threshold_; }

    const std::shared_ptr<MatchTensorScanIndexOptions> &index_options() const { return index_options_; }

private:
    std::shared_ptr<MatchTensorExpression> src_match_tensor_expr_;

    // real MatchTensorExpression used in calculation
    std::shared_ptr<void> calc_match_tensor_aligned_holder_;
    std::unique_ptr<MatchTensorExpression> calc_match_tensor_expr_holder_;
    MatchTensorExpression *calc_match_tensor_expr_ = nullptr;

    // extra options from match_tensor_expr_
    u32 topn_ = 0;
    std::optional<f32> knn_threshold_;
    std::shared_ptr<MatchTensorScanIndexOptions> index_options_;

    // column to search
    ColumnID search_column_id_ = 0;

    std::unique_ptr<std::vector<BlockMeta *>> block_metas_{};
    std::unique_ptr<TableIndexMeta> table_index_meta_{};
    std::unique_ptr<std::vector<std::shared_ptr<SegmentIndexMeta>>> segment_index_metas_{};

    mutable std::atomic_uint32_t task_executed_ = 0;

    void ExecuteInner(QueryContext *query_context, MatchTensorScanOperatorState *operator_state) const;
};

struct MatchTensorRerankDoc;
class BufferManager;
export void CalculateFusionMatchTensorRerankerScores(std::vector<MatchTensorRerankDoc> &rerank_docs,
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
