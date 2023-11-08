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
import index_data;

export module index_entry;

namespace infinity {

class SegmentEntry;
class FaissIndexPtr;
class AnnIVFFlatIndexPtr;
class BufferManager;

export class IndexEntry : public BaseEntry {
private:
    explicit IndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferObj *buffer);

public:
    static SharedPtr<IndexEntry> NewIndexEntry(SegmentEntry *segment_entry,
                                               SharedPtr<String> index_name,
                                               TxnTimeStamp create_ts,
                                               BufferManager *buffer_manager,
                                               FaissIndexPtr *index);
    static SharedPtr<IndexEntry> NewAnnIVFFlatIndexEntry(SegmentEntry *segment_entry,
                                                         SharedPtr<String> index_name,
                                                         TxnTimeStamp create_ts,
                                                         BufferManager *buffer_manager,
                                                         AnnIVFFlatIndexPtr *index_ptr);

private:
    // Load from disk. Is called by IndexEntry::Deserialize.
    static SharedPtr<IndexEntry> LoadIndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferManager *buffer_manager);

public:
    [[nodiscard]] static BufferHandle GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr);

    static void UpdateIndex(IndexEntry *index_entry, TxnTimeStamp commit_ts, FaissIndexPtr *index, BufferManager *buffer_mgr);

    static bool Flush(IndexEntry *index_entry, TxnTimeStamp checkpoint_ts);

    static Json Serialize(const IndexEntry *index_entry);

    static SharedPtr<IndexEntry> Deserialize(const Json &index_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr);

    void MergeFrom(BaseEntry &other);

private:
    static String IndexFileName(const String &index_name);

    static String IndexDirName(const String &segment_entry_dir);

public:
    const SegmentEntry *segment_entry_{};
    const SharedPtr<String> index_name_{};

private:
    BufferObj *buffer_{};

    TxnTimeStamp min_ts_{0}; // Indicate the commit_ts which create this IndexEntry
    TxnTimeStamp max_ts_{0}; // Indicate the max commit_ts which update data inside this IndexEntry
    TxnTimeStamp checkpoint_ts_{0};
};

} // namespace infinity
