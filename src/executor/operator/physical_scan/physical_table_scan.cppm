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

export module infinity_core:physical_table_scan;

import :stl;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :global_block_id;
import :table_function;
import :base_table_ref;
import :meta_info;
import :load_meta;
import internal_types;
import data_type;
import :fast_rough_filter;
import :physical_scan_base;

namespace infinity {

struct BlockIndex;

export class PhysicalTableScan : public PhysicalScanBase {
public:
    explicit PhysicalTableScan(u64 id,
                               SharedPtr<BaseTableRef> base_table_ref,
                               UniquePtr<FastRoughFilterEvaluator> &&fast_rough_filter_evaluator,
                               SharedPtr<Vector<LoadMeta>> load_metas,
                               bool add_row_id = false)
        : PhysicalScanBase(id, PhysicalOperatorType::kTableScan, nullptr, nullptr, 0, base_table_ref, load_metas),
          fast_rough_filter_evaluator_(std::move(fast_rough_filter_evaluator)), add_row_id_(add_row_id) {}

    ~PhysicalTableScan() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String table_alias() const;

    u64 TableIndex() const;

    SizeT BlockEntryCount() const;

    Vector<SizeT> &ColumnIDs() const;

    bool ParallelExchange() const override { return true; }

    bool IsExchange() const override { return true; }

private:
    void ExecuteInternal(QueryContext *query_context, TableScanOperatorState *table_scan_operator_state);

private:
    UniquePtr<FastRoughFilterEvaluator> fast_rough_filter_evaluator_{};

    bool add_row_id_;
    mutable Vector<SizeT> column_ids_;
};

} // namespace infinity
