//
// Created by JinHai on 2022/7/28.
//

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
import table_collection_entry;
import block_index;

export module physical_table_scan;

namespace infinity {

//class TableScanFunction;
//class BaseTableRef;
//class TableCollectionEntry;
//class BlockIndex;

export class PhysicalTableScan : public PhysicalOperator {
public:
    explicit PhysicalTableScan(u64 id, SharedPtr<BaseTableRef> base_table_ref)
        : PhysicalOperator(PhysicalOperatorType::kTableScan, nullptr, nullptr, id), base_table_ref_(Move(base_table_ref)) {}

    ~PhysicalTableScan() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    Vector<SharedPtr<Vector<GlobalBlockID>>> PlanBlockEntries(i64 parallel_count) const;

    String table_alias() const;

    u64 TableIndex() const;

    TableCollectionEntry *TableEntry() const;

    SizeT BlockEntryCount() const;

    BlockIndex *GetBlockIndex() const;

    Vector<SizeT> &ColumnIDs() const;

    bool ParallelExchange() const override { return true; }

    bool IsExchange() const override { return true; }

private:
    void ExecuteInternal(QueryContext *query_context, TableScanInputState *table_scan_input_state, TableScanOutputState *table_scan_output_state);

private:
    SharedPtr<BaseTableRef> base_table_ref_{};
};

} // namespace infinity
