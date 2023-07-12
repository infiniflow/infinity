#pragma once

#include "common/types/internal_types.h"
#include "page.h"

#include <atomic>

namespace infinity {

// A wrapper for memory pages, containing metadata used by the buffer pool.
class PageHandle {
    static const uint8_t kDirtyFlag = 1;      // 0000 0001
    static const uint8_t kEvictionFlags = 6;  // 0000 0110
    static const uint8_t kValidFlag = 8;      // 0000 1000
    static const uint8_t kAllFlags = 255;     // 1111 1111

public:
    PageHandle();

    ~PageHandle();

    void Initialize(const PhysicalPageId page_id);

    Page GetPage() const;

    void* GetData() const;

    void SetPageId(const PhysicalPageId page_id);
    PhysicalPageId GetPageId() const;

    void Lock(const bool exclusive);
    void Unlock();

    void SetDirty();
    void UnsetDirty();
    bool IsDirty() const;

    void SetEviction(const uint8_t value);
    void UnsetEviction();
    uint8_t GetEviction() const;
    bool IsNewlyFixed() const;

    void SetValid();
    void UnsetValid();
    bool IsValid() const;

    // Unset all flags of the current frame.
    void UnsetAllFlags();

    size_t IncFixCount();
    size_t DecFixCount();
    size_t GetFixCount() const;
    size_t ClearFixCount();

private:
    void SetFlags(const uint8_t flags) {
        flags_ |= flags;
    }
    void UnsetFlags(const uint8_t flags) {
        flags_ &= ~flags;
    }

    PageBuffer data_;

    PhysicalPageId page_id_;

    RWMutex rw_locker_{};

    // Flags for dirtiness and eviction management.
    std::atomic<uint8_t> flags_;

    // A count of how many threads have the current frame as fixed.
    std::atomic<size_t> fix_count_;
};

}
