#include "page_handle.h"
#include <memory>
#include <new>
namespace infinity {

PageHandle::PageHandle() {
    data_ = PageMemoryAllocator::Allocate( 1);
    SetPageId(PhysicalPageId());
    UnsetAllFlags();
    ClearPinCount();
}

PageHandle::~PageHandle() {}

void PageHandle::Initialize(const PhysicalPageId page_id) {
    SetPageId(page_id);
    UnsetAllFlags();
    SetValid();
    ClearPinCount();
}

Page PageHandle::GetPage() const {
    return Page(GetData());
}

void* PageHandle::GetData() const {
    return reinterpret_cast<void*>(data_.get());
}

void PageHandle::SetPageId(PhysicalPageId page_id) {
    page_id_ = page_id;
}

PhysicalPageId PageHandle::GetPageId() const {
    return page_id_;
}

void PageHandle::Lock(const bool exclusive) {
    exclusive ? rw_locker_.lock() : rw_locker_.lock_shared();
}
void PageHandle::Unlock() {
    rw_locker_.unlock();
}

void PageHandle::SetDirty() {
    SetFlags(kDirtyFlag);
}
void PageHandle::UnsetDirty() {
    UnsetFlags(kDirtyFlag);
}
bool PageHandle::IsDirty() const {
    return flags_ & kDirtyFlag;
}

void PageHandle::SetEviction(const uint8_t value) {
    SetFlags(kEvictionFlags & value);
}
void PageHandle::UnsetEviction() {
    UnsetFlags(kEvictionFlags);
}
uint8_t PageHandle::GetEviction() const {
    return flags_ & kEvictionFlags;
}
bool PageHandle::IsNewlyFixed() const {
    return (GetEviction() == 0);
}

void PageHandle::SetValid() {
    SetFlags(kValidFlag);
}
void PageHandle::UnsetValid() {
    UnsetFlags(kValidFlag);
}
bool PageHandle::IsValid() const {
    return flags_ & kValidFlag;
}

void PageHandle::UnsetAllFlags() {
    flags_ = 0;
}

size_t PageHandle::IncPinCount() {
    return ++pin_count_;
}
size_t PageHandle::DecPinCount() {
    if (pin_count_ == 0) return 0;
    return --pin_count_;
}
size_t PageHandle::GetPinCount() const {
    return pin_count_;
}
size_t PageHandle::ClearPinCount() {
    return pin_count_ = 0;
}

}
