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

export module infinity_core:physical_knn_scan;

import :query_context;
import :operator_state;
import :physical_operator_type;
import :base_table_ref;
import :meta_info;
import :load_meta;
import :knn_expression;
import :common_query_filter;
import :physical_filter_scan_base;

import data_type;
import logical_type;
import internal_types;
import :value;

namespace infinity {

class BlockMeta;
class TableIndexMeeta;
class SegmentIndexMeta;

export class PhysicalKnnScan final : public PhysicalFilterScanBase {
public:
    explicit PhysicalKnnScan(u64 id,
                             std::shared_ptr<BaseTableRef> base_table_ref,
                             std::shared_ptr<KnnExpression> knn_expression,
                             const std::shared_ptr<CommonQueryFilter> &common_query_filter,
                             std::shared_ptr<std::vector<std::string>> output_names,
                             std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types,
                             u64 knn_table_index,
                             std::shared_ptr<std::vector<LoadMeta>> load_metas);

    ~PhysicalKnnScan() override;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    //    [[nodiscard]] TableInfo *table_info() const;

    [[nodiscard]] std::string TableAlias() const;

    std::vector<size_t> &ColumnIDs() const;

    size_t BlockEntryCount() const;

    void PlanWithIndex(QueryContext *query_context);

    size_t BlockScanTaskCount() const;

    size_t TaskletCount() override;

    inline bool IsKnnMinHeap() const { return knn_expression_->IsKnnMinHeap(); }

private:
    size_t GetColumnID() const;

public:
    std::shared_ptr<KnnExpression> knn_expression_{};
    std::shared_ptr<void> real_knn_query_embedding_holder_{};
    Value function_query_embedding_holder_{LogicalType::kInvalid}; // Holds embedding data from FDE function evaluation
    void *real_knn_query_embedding_ptr_ = nullptr;
    EmbeddingDataType real_knn_query_elem_type_ = EmbeddingDataType::kElemInvalid;
    EmbeddingDataType column_elem_type_ = EmbeddingDataType::kElemInvalid;
    size_t column_embedding_dimension_ = 0; // dimension of a single vector for kEmbedding and kMultiVector
    LogicalType column_logical_type_ = LogicalType::kInvalid;
    i64 real_query_embedding_dimension_ = 0; // Actual dimension of query embedding (for FDE functions)

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};

    std::vector<std::pair<u32, u32>> block_parallel_options_;
    u32 block_column_entries_size_ = 0; // need this value because block_column_entries_ will be moved into KnnScanSharedData
    u32 index_entries_size_ = 0;

    std::unique_ptr<std::vector<BlockMeta *>> block_metas_{};
    std::shared_ptr<TableIndexMeeta> table_index_meta_{};
    std::unique_ptr<std::vector<std::shared_ptr<SegmentIndexMeta>>> segment_index_metas_{};

private:
    void InitBlockParallelOption();

    template <LogicalType t>
    void ExecuteInternalByColumnLogicalType(QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state);

    template <LogicalType t, typename ColumnDataT>
    void ExecuteInternalByColumnDataType(QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state);

    template <LogicalType t, typename ColumnDataType, template <typename, typename> typename C, typename DistanceDataType>
    void ExecuteInternalByColumnDataTypeAndQueryDataType(QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state);

    template <LogicalType t, typename ColumnDataType, template <typename, typename> typename C, typename DistanceDataType>
    friend struct ExecuteDispatchHelper;
};

} // namespace infinity
