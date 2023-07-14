#include "key_page_map.h"
#include "storage/indexstore/key_encoder.h"

namespace infinity{

PhysicalPageId KeyPageMap::KeyToPageId(const Slice& key) {
    uint64_t k = ExtractHead(key);
    std::shared_lock<RWMutex> r_locker(map_mutex_);
    auto it = map_.upper_bound(k);
    --it;
    auto page_id = it->second;
    return page_id;
}

PhysicalPageId KeyPageMap::KeyToNextPageId(const Slice& key) {
    uint64_t k = ExtractHead(key);
    std::shared_lock<RWMutex> r_locker(map_mutex_);
    auto it = map_.upper_bound(k);
    PhysicalPageId page_id;
    if (it != map_.end()) page_id = it->second;
    return page_id;
}

void KeyPageMap::Insert(const Slice& key, const PhysicalPageId& page_id) {
    std::unique_lock<RWMutex> w_locker(map_mutex_);
    map_.insert2(ExtractHead(key), page_id);
}

void KeyPageMap::Remove(const Slice& key) {
    std::unique_lock<RWMutex> w_locker(map_mutex_);
    map_.erase(ExtractHead(key));
}

size_t KeyPageMap::GetNumPages() {
  std::shared_lock<RWMutex> r_locker(map_mutex_);
  return map_.size();
}

void KeyPageMap::PreallocateAndInitialize(const std::shared_ptr<PageManager>& page_mgr) {
    const PhysicalPageId page_id = page_mgr->GetPageIO()->AllocatePage();
    auto& handle = page_mgr->Pin(page_id, true);
    uint64_t max = std::numeric_limits<uint64_t>::max();
    const Page page(
        handle.GetData(), Slice(std::string(1, 0x00)),
        Slice(reinterpret_cast<const char*>(&max)));
    page_mgr->UnPin(handle, true);
    Insert(Slice(std::string(1, 0x00)), page_id);
    page_mgr->FlushDirty();
}

void KeyPageMap::ScanFilesAndInitialize(const std::shared_ptr<PageManager>& page_mgr) {
    PageBuffer page_data = PageMemoryAllocator::Allocate(1);
    Page temp_page(page_data.get());
    for (size_t offset = 0; true; ++offset) {
        PhysicalPageId page_id(0, offset);
        if (!page_mgr->GetPageIO()->ReadPage(page_id, page_data.get()).ok())
            break;
        if (!temp_page.IsOverflow() && temp_page.IsValid()) {
            Insert(temp_page.GetLowerBoundary(), page_id);
        }
    }
}

}