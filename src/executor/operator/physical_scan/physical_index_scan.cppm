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

export module infinity_core:physical_index_scan;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :physical_scan_base;
import :base_expression;
import :load_meta;
import :infinity_exception;
import :base_table_ref;
import :value;
import :knn_filter;
import :secondary_index_data;
import :fast_rough_filter;
import :global_block_id;
import :roaring_bitmap;
import :filter_expression_push_down;

import internal_types;
import data_type;

namespace infinity {

// for int range filter, x > n is equivalent to x >= n + 1
// for float range filter, x > f is equivalent to x >= std::nextafter(f, INFINITY)
// we can use this to simplify the filter

// output: only selected RowIDs
// load columns by LoadMeta
export class PhysicalIndexScan final : public PhysicalScanBase {
public:
    explicit PhysicalIndexScan(u64 id,
                               std::shared_ptr<BaseTableRef> base_table_ref,
                               std::shared_ptr<BaseExpression> index_filter,
                               std::unique_ptr<IndexFilterEvaluator> &&index_filter_evaluator,
                               std::unique_ptr<FastRoughFilterEvaluator> &&fast_rough_filter_evaluator,
                               std::shared_ptr<std::vector<LoadMeta>> load_metas,
                               std::shared_ptr<std::vector<std::string>> output_names,
                               std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types,
                               bool add_row_id = true,
                               bool cache_result = false);

    ~PhysicalIndexScan() final = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    // different from table scan:
    // table scan: one tasklet scan one block
    // index scan: one tasklet scan one segment
    size_t TaskletCount() final;

    std::vector<std::shared_ptr<std::vector<GlobalBlockID>>> PlanBlockEntries(i64) const override;

    std::vector<std::unique_ptr<std::vector<SegmentID>>> PlanSegments(u32 parallel_count) const;

    inline std::string table_alias() const { return base_table_ref_->alias_; }

    inline u64 TableIndex() const { return base_table_ref_->table_index_; }

    inline auto &FilterExpression() const { return index_filter_; }

private:
    void ExecuteInternal(QueryContext *query_context, IndexScanOperatorState *index_scan_operator_state) const;

private:
    // input from optimizer
    std::shared_ptr<BaseExpression> index_filter_{};
    std::unique_ptr<IndexFilterEvaluator> index_filter_evaluator_{};

    std::unique_ptr<FastRoughFilterEvaluator> fast_rough_filter_evaluator_{};

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
    bool add_row_id_{};
    mutable std::vector<size_t> column_ids_{};
};

} // namespace infinity
