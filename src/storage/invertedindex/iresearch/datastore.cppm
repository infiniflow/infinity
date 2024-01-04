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
import iresearch_document;
import iresearch_analyzer;
import third_party;
import data_block;
import operator_state;
import column_vector;
import query_context;
import index_def;
import catalog;
import buffer_manager;
import default_values;

export module iresearch_datastore;

namespace infinity {

export using ScoredId = Pair<float, u32>;
export using ScoredIds = Vector<ScoredId>;
static const String DEFAULT_SCORER("bm25");
static const String DEFAULT_SCORER_ARG("");
static const SizeT DEFAULT_TOPN(100);

export u32 RowID2DocID(u32 segment_id, u32 block_id, u32 block_offset) {
    u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + block_offset;
    return (segment_id << SEGMENT_OFFSET_IN_DOCID) + segment_offset + 1;
}

export RowID DocID2RowID(u32 doc_id) { return RowID((doc_id - 1) >> SEGMENT_OFFSET_IN_DOCID, (doc_id - 1) & SEGMENT_MASK_IN_DOCID); }

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

export class IRSAsync {
public:
    ~IRSAsync() { Stop(); }

    void Stop() noexcept {
        try {
            pool_0_.stop(true);
        } catch (...) {
        }
        try {
            pool_1_.stop(true);
        } catch (...) {
        }
    }

    void ClearQueue();

    template <typename T>
    void Queue(SizeT id, T &&fn);

private:
    ThreadPool pool_0_{1};
    ThreadPool pool_1_{1};
};

struct MaintenanceState;
export class IRSDataStore {
private:
    IRSDataStore();

public:
    IRSDataStore(const IRSDataStore &) = delete;
    IRSDataStore &operator=(const IRSDataStore &) = delete;
    explicit IRSDataStore(const String &table_name, const String &directory);
    ~IRSDataStore();
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

    void StopSchedule();

    void BatchInsert(TableEntry *table_entry, const IndexDef *index_def, SegmentEntry *segment_entry, BufferManager *buffer_mgr);

    void Reset();

    ViewSnapshot *GetViewSnapshot();

    int Search(IrsFilter* flt, const Map<String, String> &options, ScoredIds &result);

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
