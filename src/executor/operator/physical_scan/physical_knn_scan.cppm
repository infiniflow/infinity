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

export module physical_knn_scan;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import global_block_id;
import base_table_ref;
import table_entry;
import block_column_entry;
import segment_index_entry;
import block_index;
import load_meta;
import knn_expression;
import infinity_exception;
import internal_types;
import data_type;
import common_query_filter;
import physical_filter_scan_base;

namespace infinity {

export class PhysicalKnnScan final : public PhysicalFilterScanBase {
public:
    explicit PhysicalKnnScan(u64 id,
                             SharedPtr<BaseTableRef> base_table_ref,
                             SharedPtr<KnnExpression> knn_expression,
                             const SharedPtr<CommonQueryFilter> &common_query_filter,
                             SharedPtr<Vector<String>> output_names,
                             SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                             u64 knn_table_index,
                             SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalFilterScanBase(id, PhysicalOperatorType::kKnnScan, nullptr, nullptr, base_table_ref, common_query_filter, load_metas),
          knn_expression_(std::move(knn_expression)), output_names_(std::move(output_names)), output_types_(std::move(output_types)),
          knn_table_index_(knn_table_index) {}

    ~PhysicalKnnScan() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    [[nodiscard]] TableEntry *table_collection_ptr() const;

    [[nodiscard]] String TableAlias() const;

    Vector<SizeT> &ColumnIDs() const;

    SizeT BlockEntryCount() const;

    void PlanWithIndex(QueryContext *query_context);

    SizeT BlockScanTaskCount() const;

    SizeT TaskletCount() override;

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    inline bool IsKnnMinHeap() const { return knn_expression_->IsKnnMinHeap(); }

private:
    SizeT GetColumnID() const;

public:
    SharedPtr<KnnExpression> knn_expression_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    u64 knn_table_index_{};

    Vector<Pair<u32, u32>> block_parallel_options_;
    u32 block_column_entries_size_ = 0; // need this value because block_column_entries_ will be moved into KnnScanSharedData
    u32 index_entries_size_ = 0;
    UniquePtr<Vector<BlockColumnEntry *>> block_column_entries_{};
    UniquePtr<Vector<SegmentIndexEntry *>> index_entries_{};

private:
    void InitBlockParallelOption();

    template <typename DataType, template <typename, typename> typename C>
    void ExecuteInternal(QueryContext *query_context, KnnScanOperatorState *operator_state);
};

} // namespace infinity
