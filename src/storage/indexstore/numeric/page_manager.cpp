#include "page_manager.h"

namespace infinity{

const size_t PAGESTORE_BUFFER_POOL_SIZE = 64 * 1024 * 1024;

PageManager::PageManager(std::filesystem::path db_path) 
    :buffer_manager_size_(PAGESTORE_BUFFER_POOL_SIZE / Page::kSize)
{
    page_io_ = std::make_unique<PageIO>(std::move(db_path));
    page_cache_ = std::make_unique<PageCache>(buffer_manager_size_);
}

PageManager::~PageManager(){

}

PageHandle& PageManager::Pin(
    const PhysicalPageId page_id,
    const bool exclusive){
    PageHandle* handle = nullptr;
    {
        std::shared_lock<RWMutex> r_locker(map_mutex_);
        if(buffer_map_.find(page_id) != buffer_map_.end()){
            // Page is already loaded in buffer pool
        }
    }
    // If not, we need to load from disk
    while (handle == nullptr) {
        handle = page_cache_->Evict();
        if(handle != nullptr) {
            std::unique_lock<RWMutex> w_locker(map_mutex_);
            buffer_map_.erase(handle->GetPageId());
        }
    }

    if(handle->IsDirty()){
        FlushPage(handle);
        handle->UnsetDirty();
    }

    handle->Initialize(page_id);
    handle->IncFixCount();
    LoadPage(handle);
    {
        std::unique_lock<RWMutex> w_locker(map_mutex_);
        buffer_map_.emplace(page_id, handle);
    }

    handle->Lock(exclusive);
    return *handle;
}

void PageManager::UnPin(PageHandle& handle ,const bool is_dirty){

}

void PageManager::FlushPage(PageHandle* handle) {
    Status s = page_io_->WritePage(handle->GetPageId(), handle->GetData());
    if (!s.ok()) throw std::runtime_error("Tried to write to unallocated page.");
}

void PageManager::LoadPage(PageHandle* handle) {
  Status s = page_io_->ReadPage(handle->GetPageId(), handle->GetData());
  if (!s.ok()) throw std::runtime_error("Tried to read from unallocated page.");
}

}