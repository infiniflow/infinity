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
import new_catalog;
import table_function;
import query_context;
import data_block;
import default_values;
import table_collection_entry;
import segment_entry;

export module seq_scan;

namespace infinity {

export void RegisterSeqScanFunction(const UniquePtr<NewCatalog> &catalog_ptr);

class SeqScanFunctionData : public TableFunctionData {
public:
    SeqScanFunctionData(TableCollectionEntry *table_entry_ptr, SharedPtr<Vector<SegmentEntry *>> segment_entries, Vector<SizeT> column_ids)
        : table_entry_ptr_(table_entry_ptr), segment_entries_(Move(segment_entries)), column_ids_(Move(column_ids)) {}

    TableCollectionEntry *table_entry_ptr_;
    Vector<SizeT> column_ids_;

    // How many segments are scanned.
    SharedPtr<Vector<SegmentEntry *>> segment_entries_{};

    i64 segment_count_{0};

    i64 current_segment_id_{INVALID_SEGMENT_ID};
    i64 current_block_id{INVALID_BLOCK_ID};
    SizeT read_offset_{0};
};

class SeqScanFunction : public TableFunction {
public:
    static SharedPtr<SeqScanFunction> Make(NewCatalog *catalog, const String &func_name);

public:
    explicit SeqScanFunction(String name, TableFunctionType function) : TableFunction(Move(name), {}, Move(function)) {}

private:
};

static void SeqScanFunc(QueryContext *query_context, TableFunctionData *table_function_data_ptr, DataBlock &output);

} // namespace infinity