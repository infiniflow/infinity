#pragma once

#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"
#include "storage/buffer/buffer_handle.h"
#include "storage/buffer/object_handle.h"
#include "storage/meta/entry/base_entry.h"

namespace infinity {

class BufferManager;
class SegmentEntry;
class FaissIndexPtr;

class IndexEntry : BaseEntry {
private:
    explicit IndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferHandle *buffer_handle)
        : BaseEntry(EntryType::kIndex), segment_entry_(segment_entry), index_name_(std::move(index_name)), buffer_handle_(buffer_handle){};

public:
    static SharedPtr<IndexEntry>
    NewIndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferManager *buffer_manager, FaissIndexPtr *index);

private:
    static SharedPtr<IndexEntry> LoadIndexEntry(SegmentEntry *segment_entry, SharedPtr<String> index_name, BufferManager *buffer_manager);

public:
    [[nodiscard]] static ObjectHandle GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr);

    static void Flush(const IndexEntry *index_entry);

    static nlohmann::json Serialize(const IndexEntry *index_entry);

    static SharedPtr<IndexEntry> Deserialize(const nlohmann::json &index_entry_json, SegmentEntry *segment_entry, BufferManager *buffer_mgr);

private:
    static inline String IndexFileName(const String &index_name) { return index_name + ".idx"; }

    static inline String IndexDirName(const String &segment_entry_dir) { return segment_entry_dir + "/index"; }

public:
    const SegmentEntry *segment_entry_{};
    const SharedPtr<String> index_name_{};

private:
    BufferHandle *buffer_handle_{};
};
} // namespace infinity