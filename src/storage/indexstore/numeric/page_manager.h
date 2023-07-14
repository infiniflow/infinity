#pragma once

#include <filesystem>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <tuple>

#include "page_handle.h"
#include "page_io.h"
#include "page_cache.h"
#include "common/types/internal_types.h"


namespace infinity{

class PageManager{
public:
    PageManager(std::filesystem::path db_path); 

    ~PageManager();

    PageHandle& Pin(const PhysicalPageId page_id, const bool exclusive);

    void UnPin(PageHandle& handle, const bool is_dirty);

    bool Contains(const PhysicalPageId page_id);

    PageIO* GetPageIO() const { return page_io_.get(); }

    void FlushDirty(bool require_delete = false);
private:
    void FlushPage(PageHandle* handle);

    void LoadPage(PageHandle* handle);

    // The number of pages kept in memory.
    std::atomic<size_t> buffer_manager_size_;

    HashMap<PhysicalPageId, PageHandle*> buffer_map_;

    std::unique_ptr<PageCache> page_cache_;

    RWMutex map_mutex_;

    std::unique_ptr<PageIO> page_io_;
};

}