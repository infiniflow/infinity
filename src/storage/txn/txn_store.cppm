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
import data_access_state;
import index_entry;

export module txn_store;

namespace infinity {

class Txn;
class TableCollectionEntry;
class SegmentEntry;
class DataBlock;

export class TxnTableStore {
public:
    explicit inline TxnTableStore(TableCollectionEntry *table_entry, Txn *txn) : table_entry_(table_entry), txn_(txn) {}

    UniquePtr<String> Append(const SharedPtr<DataBlock> &input_block);

    UniquePtr<String> Import(const SharedPtr<SegmentEntry> &segment);

    UniquePtr<String> CreateIndexFile(u32 segment_id, SharedPtr<IndexEntry> index);

    UniquePtr<String> Delete(const Vector<RowID> &row_ids);

    void Scan(SharedPtr<DataBlock> &output_block);

    void Rollback();

    void PrepareCommit();

    void Commit() const;

public:
    Vector<SharedPtr<DataBlock>> blocks_{};
    Vector<SharedPtr<SegmentEntry>> uncommitted_segments_{};
    HashMap<u32, SharedPtr<IndexEntry>> uncommitted_indexes_{};
    UniquePtr<AppendState> append_state_{};
    DeleteState delete_state_{};

    SizeT current_block_id_{0};

    TableCollectionEntry *table_entry_{};
    Txn *txn_{};
};

} // namespace infinity
