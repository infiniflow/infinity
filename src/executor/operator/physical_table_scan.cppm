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
import global_block_id;
import table_function;
import base_table_ref;
import catalog;
import block_index;
import load_meta;

export module physical_table_scan;

namespace infinity {

// class TableScanFunction;
// class BaseTableRef;
// struct TableEntry;
// class BlockIndex;

export class PhysicalTableScan : public PhysicalOperator {
public:
    explicit PhysicalTableScan(u64 id, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<Vector<LoadMeta>> load_metas, bool add_row_id = false)
        : PhysicalOperator(PhysicalOperatorType::kTableScan, nullptr, nullptr, id, load_metas), base_table_ref_(Move(base_table_ref)),
          add_row_id_(add_row_id) {}

    ~PhysicalTableScan() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SizeT TaskletCount() override;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    Vector<SharedPtr<Vector<GlobalBlockID>>> PlanBlockEntries(i64 parallel_count) const;

    String table_alias() const;

    u64 TableIndex() const;

    TableEntry *TableEntry() const;

    SizeT BlockEntryCount() const;

    BlockIndex *GetBlockIndex() const;

    Vector<SizeT> &ColumnIDs() const;

    bool ParallelExchange() const override { return true; }

    bool IsExchange() const override { return true; }

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

private:
    void ExecuteInternal(QueryContext *query_context, TableScanOperatorState *table_scan_operator_state);

private:
    SharedPtr<BaseTableRef> base_table_ref_{};

    bool add_row_id_;
    mutable Vector<SizeT> column_ids_;
};

} // namespace infinity
