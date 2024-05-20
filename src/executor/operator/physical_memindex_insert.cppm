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

export module physical_memindex_insert;

import stl;
import physical_operator;
import physical_operator_type;
import load_meta;
import data_type;
import operator_state;
import query_context;
import column_vector;
import column_inverter;

namespace infinity {

class MemoryIndexer;

export class PhysicalMemIndexInsert : public PhysicalOperator {
public:
    PhysicalMemIndexInsert(u64 id,
                           MemoryIndexer *memory_indexer,
                           SharedPtr<ColumnInverter> inverter,
                           SharedPtr<ColumnVector> column_vector,
                           u32 row_offset,
                           u32 row_count,
                           u32 start_doc_id,
                           u64 task_seq,
                           bool offline,
                           SharedPtr<Vector<LoadMeta>> load_metas,
                           SharedPtr<Vector<String>> output_names,
                           SharedPtr<Vector<SharedPtr<DataType>>> output_types)
        : PhysicalOperator(PhysicalOperatorType::kMemIndexInsert, nullptr, nullptr, id, load_metas), memory_indexer_(memory_indexer),
          inverter_(inverter), column_vector_(column_vector), row_offset_(row_offset), row_count_(row_count), start_doc_id_(start_doc_id),
          task_seq_(task_seq), offline_(offline), output_names_(output_names), output_types_(output_types) {}

    ~PhysicalMemIndexInsert() override = default;

    void Init() override {}

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SizeT TaskletCount() override { return 1; }

    SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    MemoryIndexer *memory_indexer_;
    SharedPtr<ColumnInverter> inverter_;
    SharedPtr<ColumnVector> column_vector_;
    u32 row_offset_;
    u32 row_count_;
    u32 start_doc_id_;
    u64 task_seq_;
    bool offline_;

private:
    SharedPtr<Vector<String>> output_names_;
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_;
};

} // namespace infinity