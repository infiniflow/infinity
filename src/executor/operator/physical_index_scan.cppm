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

export module physical_index_scan;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import load_meta;
import infinity_exception;
import internal_types;
import data_type;
import base_table_ref;
import value;
import knn_filter;
import secondary_index_data;
import secondary_index_scan_execute_expression;
import table_index_entry;
import segment_index_entry;
import fast_rough_filter;
import bitmask;

namespace infinity {

// for int range filter, x > n is equivalent to x >= n + 1
// for float range filter, x > f is equivalent to x >= std::nextafter(f, INFINITY)
// we can use this to simplify the filter

// output: only selected RowIDs
// load columns by LoadMeta
export class PhysicalIndexScan final : public PhysicalOperator {
public:
    explicit PhysicalIndexScan(u64 id,
                               SharedPtr<BaseTableRef> base_table_ref,
                               SharedPtr<BaseExpression> index_filter_qualified,
                               HashMap<ColumnID, TableIndexEntry *> &&column_index_map,
                               Vector<FilterExecuteElem> &&filter_execute_command,
                               UniquePtr<FastRoughFilterEvaluator> &&fast_rough_filter_evaluator,
                               SharedPtr<Vector<LoadMeta>> load_metas,
                               bool add_row_id = true);

    ~PhysicalIndexScan() final = default;

    void Init() final;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    // different from table scan:
    // table scan: one tasklet scan one block
    // index scan: one tasklet scan one segment
    SizeT TaskletCount() final { return base_table_ref_->block_index_->SegmentCount(); }

    // for InputLoad
    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    Vector<UniquePtr<Vector<SegmentID>>> PlanSegments(u32 parallel_count) const;

    inline String table_alias() const { return base_table_ref_->alias_; }

    inline u64 TableIndex() const { return base_table_ref_->table_index_; }

    inline auto *TableEntry() const { return base_table_ref_->table_entry_ptr_; }

    inline auto &FilterExpression() const { return index_filter_qualified_; }

private:
    void ExecuteInternal(QueryContext *query_context, IndexScanOperatorState *index_scan_operator_state) const;

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    SharedPtr<BaseTableRef> base_table_ref_{};
    // input from optimizer
    SharedPtr<BaseExpression> index_filter_qualified_{};
    HashMap<ColumnID, TableIndexEntry *> column_index_map_{};
    // Commands used in ExecuteInternal()
    Vector<FilterExecuteElem> filter_execute_command_{};

    UniquePtr<FastRoughFilterEvaluator> fast_rough_filter_evaluator_{};

    bool add_row_id_{};
    mutable Vector<SizeT> column_ids_{};
};

export std::variant<Vector<u32>, Bitmask> SolveSecondaryIndexFilter(const Vector<FilterExecuteElem> &filter_execute_command,
                                                                    const HashMap<ColumnID, TableIndexEntry *> &column_index_map,
                                                                    const SegmentID segment_id,
                                                                    const u32 segment_row_count,
                                                                    const u32 segment_row_actual_count,
                                                                    const TxnTimeStamp ts);

} // namespace infinity
