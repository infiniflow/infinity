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
import iresearch_document;
import iresearch_analyzer;
import third_party;
import data_block;
import operator_state;
import column_vector;
import query_context;
import index_def;
import segment_entry;
import table_collection_entry;
import buffer_manager;

export module iresearch_datastore;

namespace infinity {

export struct ViewSegment {
    ViewSegment(const IRSSubReader &segment) : segment_(&segment) {}
    const IRSSubReader *segment_;
};

export class ViewSnapshot {
public:
    ViewSnapshot(IRSDirectoryReader *reader);

    const IRSSubReader *operator[](SizeT i) noexcept;

    const ViewSegment &GetSegment(SizeT i) noexcept;

public:
    Vector<ViewSegment> segments_;
    IRSDirectoryReader *reader_;
};

class IRSAsync;
struct MaintenanceState;
export class IRSDataStore {
public:
    explicit IRSDataStore(const String &table_name, const String &directory);

    struct DataSnapshot {
        DataSnapshot(IRSDirectoryReader &&reader) : reader_(Move(reader)) {}
        DataSnapshot &operator=(DataSnapshot &&rhs) noexcept {
            if (this != &rhs) {
                reader_ = Move(rhs.reader_);
            }
            return *this;
        }

        IRSDirectoryReader reader_;
    };
    using DataSnapshotPtr = SharedPtr<IRSDataStore::DataSnapshot>;

    IRSIndexWriter::ptr GetWriter() { return index_writer_; }

    auto const &GetDirectoryReader() const { return snapshot_->reader_; }

    void StoreSnapshot(DataSnapshotPtr snapshot);

    IRSDataStore::DataSnapshotPtr LoadSnapshot() const;

    void Commit();

    void ScheduleCommit();

    void ScheduleConsolidation();

    void ScheduleOptimize();

    void BatchInsert(TableCollectionEntry *table_entry, IndexDef *index_def, SegmentEntry *segment_entry, BufferManager *buffer_mgr);

    void Reset();

    ViewSnapshot *GetViewSnapshot();

private:
    String directory_;
    Path path_;
    IRSDirectory::ptr irs_directory_;
    IRSIndexWriter::ptr index_writer_;
    IRSIndexWriter::Transaction recovery_txn_;
    Mutex commit_mutex_;
    DataSnapshotPtr snapshot_;
    UniquePtr<IRSAsync> async_;
    SharedPtr<MaintenanceState> maintenance_state_;
};

} // namespace infinity
