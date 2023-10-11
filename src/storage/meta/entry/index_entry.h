#pragma once

#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"
#include "storage/buffer/buffer_handle.h"
#include "storage/buffer/object_handle.h"

namespace infinity {

class BufferManager;

class SegmentEntry;

class IndexEntry {
public:
    explicit IndexEntry(SharedPtr<String> segment_entry_dir, SharedPtr<String> index_name, BufferHandle *buffer_handle)
        : segment_entry_dir_(std::move(segment_entry_dir)), index_name_(std::move(index_name)), buffer_handle_(buffer_handle){};

public:
    static UniquePtr<IndexEntry> MakeNewIndexEntry(SharedPtr<String> segment_dir, SharedPtr<String> index_name, BufferManager *buffer_manager);

    // T is the index type. such as faiss::Index. The function will return a TemplateHandle that loads T*
    [[nodiscard]] static ObjectHandle GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr);

    static void Flush(IndexEntry *index_entry);

private:
    static inline String IndexFileName(const String &index_name) { return index_name + ".idx"; }

    static inline String IndexDirName(const String &segment_entry_dir) { return segment_entry_dir + "/index"; }

private:
    SharedPtr<String> segment_entry_dir_{};
    SharedPtr<String> index_name_{};
    BufferHandle *buffer_handle_{};
};
} // namespace infinity