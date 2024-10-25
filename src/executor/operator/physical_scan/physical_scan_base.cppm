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

export module physical_scan_base;

import stl;
import physical_operator;
import physical_operator_type;
import global_block_id;
import base_table_ref;
import block_index;
import load_meta;
import internal_types;
import operator_state;
import query_context;
import table_entry;

namespace infinity {

class ResultCacheManager;
class DataBlock;

export class PhysicalScanBase : public PhysicalOperator {
public:
    PhysicalScanBase(const u64 id,
                     const PhysicalOperatorType type,
                     UniquePtr<PhysicalOperator> left,
                     UniquePtr<PhysicalOperator> right,
                     u64 table_index,
                     SharedPtr<BaseTableRef> base_table_ref,
                     SharedPtr<Vector<LoadMeta>> load_metas,
                     bool cache_result = false)
        : PhysicalOperator(type, std::move(left), std::move(right), id, std::move(load_metas), cache_result), table_index_(table_index),
          base_table_ref_(std::move(base_table_ref)) {}

    virtual Vector<SharedPtr<Vector<GlobalBlockID>>> PlanBlockEntries(i64 parallel_count) const;

    SizeT TaskletCount() override;

    virtual BlockIndex *GetBlockIndex() const;

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    u64 table_index() const { return table_index_; }

    [[nodiscard]] inline String TableAlias() const { return base_table_ref_->alias_; }

    [[nodiscard]] inline TableEntry *table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

protected:
    void SetOutput(const Vector<char *> &raw_result_dists_list,
                   const Vector<RowID *> &row_ids_list,
                   SizeT result_size,
                   i64 result_n,
                   QueryContext *query_context,
                   OperatorState *operator_state);

    void AddCache(QueryContext *query_context, ResultCacheManager *cache_mgr, const Vector<UniquePtr<DataBlock>> &output_data_blocks) const;

public:
    u64 table_index_ = 0;
    SharedPtr<BaseTableRef> base_table_ref_{};
};

} // namespace infinity
