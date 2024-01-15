module;

#include <atomic>

import stl;
module atomic_entry_ref;

namespace infinity {
AtomicEntryRef::AtomicEntryRef() noexcept : ref_() {}

AtomicEntryRef::AtomicEntryRef(EntryRef ref) noexcept : ref_(ref.Ref()) {}

AtomicEntryRef::AtomicEntryRef(const AtomicEntryRef &rhs) noexcept : ref_(rhs.ref_.load(std::memory_order_relaxed)) {}

AtomicEntryRef::AtomicEntryRef(AtomicEntryRef &&rhs) noexcept : ref_(rhs.ref_.load(std::memory_order_relaxed)) {}

AtomicEntryRef &AtomicEntryRef::operator=(const AtomicEntryRef &rhs) noexcept {
    u32 ref = rhs.ref_.load(std::memory_order_relaxed);
    ref_.store(ref, std::memory_order_relaxed);
    return *this;
}

void AtomicEntryRef::StoreRelease(EntryRef ref) noexcept { ref_.store(ref.Ref(), std::memory_order_release); }

void AtomicEntryRef::StoreRelaxed(EntryRef ref) noexcept { ref_.store(ref.Ref(), std::memory_order_relaxed); }

EntryRef AtomicEntryRef::LoadAcquire() const noexcept { return EntryRef(ref_.load(std::memory_order_acquire)); }

EntryRef AtomicEntryRef::LoadRelaxed() const noexcept { return EntryRef(ref_.load(std::memory_order_relaxed)); }

} // namespace infinity
