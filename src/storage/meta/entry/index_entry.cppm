
module;

import stl;
import base_entry;
import buffer_handle;
import third_party;
import buffer_obj;

export module index_entry;

namespace infinity {

class SegmentEntry;
class FaissIndexPtr;
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

private:
    // Load from disk. Is called by IndexEntry::Deserialize.
    static SharedPtr<IndexEntry> LoadIndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferManager *buffer_manager);

public:
    [[nodiscard]] static BufferHandle GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr);

    [[nodiscard]] static BufferHandleMut GetIndexMut(IndexEntry *index_entry, BufferManager *buffer_mgr);

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
