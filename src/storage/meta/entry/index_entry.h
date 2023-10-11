#pragma once

#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"
#include "storage/buffer/buffer_handle.h"
#include "storage/index_def/index_def.h"
namespace infinity {

class BufferManager;

class IndexEntry {
public:
    explicit IndexEntry(SharedPtr<String> segment_entry_dir, SharedPtr<IndexDef> index_def)
        : segment_entry_dir_(std::move(segment_entry_dir)), index_def_(std::move(index_def)) {}

public:
    [[nodiscard]] static const void GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr);

private:
    static inline String IndexFileName(const String &index_name) { return index_name + ".idx"; }

    static inline String IndexDirName(const String &segment_entry_dir) { return segment_entry_dir + "/index"; }

private:
    SharedPtr<String> segment_entry_dir_{};
    SharedPtr<IndexDef> index_def_{};
    BufferHandle *buffer_handle_{};
};
} // namespace infinity