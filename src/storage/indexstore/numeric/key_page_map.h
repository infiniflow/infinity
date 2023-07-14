#pragma once

#include "page_manager.h"
#include <leveldb/slice.h>
#include <third_party/tlx/btree_map.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace infinity {

class KeyPageMap {
public:
    KeyPageMap() = default;
    ~KeyPageMap() = default;

    PhysicalPageId KeyToPageId(const Slice& key);

    PhysicalPageId KeyToNextPageId(const Slice& key);

    void Insert(const Slice& key, const PhysicalPageId& page_id);

    void Remove(const Slice& key);

    size_t GetNumPages();

    void PreallocateAndInitialize(const std::shared_ptr<PageManager>& page_mgr);

    void ScanFilesAndInitialize(const std::shared_ptr<PageManager>& page_mgr);
private:
    tlx::btree_map<uint64_t, PhysicalPageId, std::less<uint64_t>> map_;

    RWMutex map_mutex_;
};

}
