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
import base_entry;
import buffer_handle;
import third_party;
import buffer_obj;
import parser;
import index_file_worker;

export module index_entry;

namespace infinity {

class IndexDefEntry;
class FaissIndexPtr;
class BufferManager;
class IndexDef;
class SegmentEntry;

export class IndexEntry : public BaseEntry {
private:
    explicit IndexEntry(IndexDefEntry *index_def_entry, SegmentEntry *segment_entry, BufferObj *buffer);

public:
    static SharedPtr<IndexEntry> NewIndexEntry(IndexDefEntry *index_def_entry,
                                               SegmentEntry *segment_entry,
                                               TxnTimeStamp create_ts,
                                               BufferManager *buffer_manager,
                                               UniquePtr<CreateIndexPara> create_index_para);

private:
    // Load from disk. Is called by IndexEntry::Deserialize.
    static SharedPtr<IndexEntry> LoadIndexEntry(IndexDefEntry *index_def_entry,
                                                SegmentEntry *segment_entry,
                                                BufferManager *buffer_manager,
                                                UniquePtr<CreateIndexPara> create_index_para);

public:
    [[nodiscard]] static BufferHandle GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr);

    static void UpdateIndex(IndexEntry *index_entry, TxnTimeStamp commit_ts, FaissIndexPtr *index, BufferManager *buffer_mgr);

    static bool Flush(IndexEntry *index_entry, TxnTimeStamp checkpoint_ts);

    static Json Serialize(const IndexEntry *index_entry);

    static SharedPtr<IndexEntry> Deserialize(const Json &index_entry_json,
                                             IndexDefEntry *index_def_entry_,
                                             SegmentEntry *segment_entry,
                                             BufferManager *buffer_mgr,
                                             UniquePtr<CreateIndexPara> create_index_para);

    void MergeFrom(BaseEntry &other);

private:
    static UniquePtr<IndexFileWorker> CreateFileWorker(IndexDefEntry *index_def_entry, UniquePtr<CreateIndexPara> para);

    static String IndexFileName(const String &index_name);

public:
    IndexDefEntry *const index_def_entry_;
    SegmentEntry *const segment_entry_;

private:
    BufferObj *const buffer_{};

    TxnTimeStamp min_ts_{0}; // Indicate the commit_ts which create this IndexEntry
    TxnTimeStamp max_ts_{0}; // Indicate the max commit_ts which update data inside this IndexEntry
    TxnTimeStamp checkpoint_ts_{0};
};

} // namespace infinity
