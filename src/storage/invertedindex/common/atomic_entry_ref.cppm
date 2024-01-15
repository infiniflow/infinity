module;

import stl;
export module atomic_entry_ref;

namespace infinity {

export class EntryRef {
protected:
    u32 ref_;

public:
    EntryRef() noexcept : ref_(0u) {}
    explicit EntryRef(u32 ref_) noexcept : ref_(ref_) {}
    u32 Ref() const noexcept { return ref_; }
    u32 Hash() const noexcept { return ref_; }
    bool IsValid() const noexcept { return ref_ != 0u; }
    u32 BufferId(u32 offset_bits) const noexcept { return ref_ >> offset_bits; }
    bool operator==(const EntryRef &rhs) const noexcept { return ref_ == rhs.ref_; }
    bool operator!=(const EntryRef &rhs) const noexcept { return ref_ != rhs.ref_; }
    bool operator<(const EntryRef &rhs) const noexcept { return ref_ < rhs.ref_; }
    bool operator<=(const EntryRef &rhs) const noexcept { return ref_ <= rhs.ref_; }
};

//
// A wrapper for std::atomic of type EntryRef that supports copy and move constructors and assignment operator,
// and uses Release-Acquire ordering for store and load.
export class AtomicEntryRef {
public:
    AtomicEntryRef() noexcept;
    explicit AtomicEntryRef(EntryRef ref) noexcept;
    AtomicEntryRef(const AtomicEntryRef &rhs) noexcept;
    AtomicEntryRef(AtomicEntryRef &&rhs) noexcept;
    AtomicEntryRef &operator=(const AtomicEntryRef &rhs) noexcept;
    void StoreRelease(EntryRef ref) noexcept;
    void StoreRelaxed(EntryRef ref) noexcept;
    EntryRef LoadAcquire() const noexcept;
    EntryRef LoadRelaxed() const noexcept;

private:
    Atomic<u32> ref_;
};

} // namespace infinity
