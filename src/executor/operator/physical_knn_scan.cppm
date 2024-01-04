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

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import global_block_id;
import base_table_ref;
import catalog;
import block_index;
import load_meta;
import knn_expression;
import infinity_exception;

export module physical_knn_scan;

namespace infinity {

export class PhysicalKnnScan final : public PhysicalOperator {
public:
    explicit PhysicalKnnScan(u64 id,
                             SharedPtr<BaseTableRef> base_table_ref,
                             SharedPtr<KnnExpression> knn_expression,
                             SharedPtr<BaseExpression> filter_expression,
                             SharedPtr<Vector<String>> output_names,
                             SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                             u64 knn_table_index,
                             SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kKnnScan, nullptr, nullptr, id, load_metas), base_table_ref_(Move(base_table_ref)),
          knn_expression_(Move(knn_expression)), filter_expression_(Move(filter_expression)), output_names_(Move(output_names)),
          output_types_(Move(output_types)), knn_table_index_(knn_table_index) {}

    ~PhysicalKnnScan() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    [[nodiscard]] TableEntry *table_collection_ptr() const;

    [[nodiscard]] String TableAlias() const;

    BlockIndex *GetBlockIndex() const;

    Vector<SizeT> &ColumnIDs() const;

    SizeT BlockEntryCount() const;

    void PlanWithIndex(QueryContext *query_context);

    inline SizeT TaskCount() const {
        return block_column_entries_->size() + index_entries_->size();
    }

    SizeT TaskletCount() override {
        return block_column_entries_->size() + index_entries_->size();
    }

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

public:
    SharedPtr<BaseTableRef> base_table_ref_{};

    SharedPtr<KnnExpression> knn_expression_{};

    SharedPtr<BaseExpression> filter_expression_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    u64 knn_table_index_{};

    UniquePtr<Vector<BlockColumnEntry *>> block_column_entries_{};
    UniquePtr<Vector<SegmentColumnIndexEntry *>> index_entries_{};

private:
    template <typename DataType, template <typename, typename> typename C>
    void ExecuteInternal(QueryContext *query_context, KnnScanOperatorState *operator_state);
};

} // namespace infinity
