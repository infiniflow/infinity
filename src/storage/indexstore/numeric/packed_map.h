// Acknowledgement: The code in this file is adapted from code originally written by Viktor Leis.

#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>

#include "page_id.h"

namespace infinity {

// An ordered map with a configurable compile-time fixed size that is strictly
// less than 64 KiB.
//
// Keys and payloads are treated as arbitrary bytes. This map keeps keys ordered
// lexicographically. Keys have a maximum size, specified by
// `PackedMap<>::kMaxKeySizeBytes`. The key range must also be specified up
// front to leverage prefixed encoding.
//
// Since this map can become full, it provides an eight byte buffer to store
// optional "overflow" information (e.g., a pointer, offset, or identifier for
// an additional map). This buffer must be managed by the map user.
//
// This map is not thread safe; it requires external mutual exclusion.
template <uint16_t MapSizeBytes>
class PackedMap {
    // We want `sizeof(PackedMap<MapSizeBytes>) == MapSizeBytes` to be true to
    // make it easier to reason about this class. If the requested map size is not
    // divisible by 8 (should be architecture-specific), the compiler will add
    // extra padding to this class. For simplicity, we require `MapSizeBytes` to
    // be divisible by 8.
    static_assert(MapSizeBytes % 8 == 0,
                  "The PackedMap must have a size that is divisible by 8.");

public:
    // Create a map without key bounds. The `PackedMap` will not use prefix
    // encoding in this case.
    PackedMap() = default;

    // Create a map with a fixed key bound. This enables more efficient encoding
    // of later inserted keys, but requires that all inserted keys satisfy:
    // `lower_key` <= key <= `upper_key`. Using this constructor and then
    // violating this ordering constraint results in undefined behavior.
    // Can set upper_key_length = 0 to indicate an unknown upper key, forgoing
    // the more efficient encoding.
    PackedMap(const uint8_t* lower_key, unsigned lower_key_length,
              const uint8_t* upper_key, unsigned upper_key_length);

    // Insert `key` and `payload` and return true iff the insert succeeded.
    // Duplicate key insertions will just overwrite the payload.
    bool Insert(const uint8_t* key, unsigned key_length, const uint8_t* payload,
                unsigned payload_length);

    // Update `key`, which must already be in the map, with a new `payload` and
    // return true. If there is not enough space, then simply remove the pair
    // (`key`, old_payload) if it exists and return false.
    bool UpdateOrRemove(const uint8_t* key, unsigned key_length,
                        const uint8_t* payload, unsigned payload_length);

    // Insert `key` and `payload` and return true iff the insert succeeded. `key`
    // must be lexicographically greater than the largest key currently in the
    // map.
    //
    // If `perform_checks` is set to true, this key sorting condition will be
    // checked by calling CanAppend(), and Insert() or HandleDuplicateInsertion()
    // will be called instead if necessary. Otherwise, it is the user's
    // responsibility to maintain this requirement and undefined behavior will
    // result from violating it.
    //
    // Recommended for bulk insertions of unique keys in sorted order.
    //
    // Like Insert(), this function treats key re-insertions as overwrites. This
    // is consistent with what LevelDB/RocksDB do. To support non-unique keys, we
    // would need to decide on the desired behavior of Get() and Remove() when
    // dealing with keys with multiple associated values.
    bool Append(const uint8_t* key, unsigned key_length, const uint8_t* payload,
                unsigned payload_length, bool perform_checks);

    // Checks whether Append() can be used efficiently by lexicographically
    // comparing `key` to the largest key currently in the map.
    //
    // Returns `val` with:
    // -- val < 0 if `key` is smaller than the largest key currently in the map.
    // -- val == 0 if `key` is equal to the largest key currently in the map.
    // -- val > 0 if 'key` is larger than the largest key currently in the map.
    int8_t CanAppend(const uint8_t* key, unsigned key_length);

    // Remove the record associated with the specified key and return true if
    // the removal succeeded.
    bool Remove(const uint8_t* key, unsigned key_length);

    // Retrieve the payload for the specified key, returning true if found. If
    // the key is not found, `*payload_out` will be set to `nullptr` and
    // `*payload_length_out` will be set to 0.
    bool Get(const uint8_t* key, unsigned key_length, const uint8_t** payload_out,
             unsigned* payload_length_out) const;

    // Retrieve the stored `overflow` (an arbitrary page id managed by
    // the map user).
    PhysicalPageId GetOverflow() const;

    // Set the stored overflow value to `overflow` (an arbitrary page id
    // managed by the map user).
    void SetOverflow(PhysicalPageId overflow);

    // Returns the number of records currently stored in this map.
    uint16_t GetNumRecords() const;

    // Retrieves the common prefix among all keys stored in this map.
    void GetKeyPrefix(const uint8_t** key_prefix_out,
                      unsigned* key_prefix_length_out) const;

    // Retrieve the key suffix stored in the given `slot_id`, returning true if
    // successful. Note that the full key is the prefix given by `GetKeyPrefix()`
    // concatenated with the suffix retrieved by this method.
    //
    // If there is no record at the provided `slot_id` (e.g., if `slot_id >=
    // GetNumRecords()`), this method will return false.
    bool GetKeySuffixInSlot(uint16_t slot_id, const uint8_t** key_suffix_out,
                            unsigned* key_suffix_length_out) const;

    // Retrieve the payload stored in the given `slot_id`, returning true if
    // successful.
    //
    // If there is no record at the provided `slot_id` (e.g., if `slot_id >=
    // GetNumRecords()`), this method will return false.
    bool GetPayloadInSlot(uint16_t slot_id, const uint8_t** payload_out,
                          unsigned* payload_length_out) const;

    // Finds the `slot_id` containing the first record with a key that is greater
    // than or equal to `key`.
    //
    // If no such record exists (e.g., if this map is empty or all record keys are
    // less than `key`), the returned `slot_id` will be greater than or equal to
    // the return value of `GetNumRecords()`.
    uint16_t LowerBoundSlot(const uint8_t* key, unsigned key_length) const;

    // Access the fences and their lengths - useful for creating a new PackedMap
    // with the same key range.
    const uint8_t* GetLowerFence() const;
    const uint8_t* GetUpperFence() const;
    const uint16_t GetLowerFenceLength() const;
    const uint16_t GetUpperFenceLength() const;

    // Check whether this is a valid PackedMap (as opposed to a PackedMap-sized
    // block of 0s).
    const bool IsValid() const;

    // Check whether this is an overflow page & make/unmake it one.
    const bool IsOverflow() const;
    void MakeOverflow();
    void UnmakeOverflow();

private:
    static constexpr unsigned kHintCount = 16;

    static constexpr uint8_t kValidFlag = 1;
    static constexpr uint8_t kOverflowFlag = 2;

    struct Header {
        struct FenceKeySlot {
            uint16_t offset;
            uint16_t length;
        };
        // If non-zero, these fences delineate the key space in this map:
        // [lower_fence, upper_fence]. That is, all keys in this map must satisfy
        // lower_fence <= key <= upper_fence.
        FenceKeySlot lower_fence = {0, 0};
        FenceKeySlot upper_fence = {0, 0};

        uint16_t count = 0;
        uint16_t space_used = 0;
        uint16_t data_offset = MapSizeBytes;
        uint16_t prefix_length = 0;

        uint32_t hint[kHintCount];
        PhysicalPageId overflow;

        uint8_t flags = kValidFlag & ~kOverflowFlag;
    };
    struct Slot {
        uint16_t offset;
        uint16_t key_length;
        uint16_t payload_length;
        union {
            uint32_t head;
            uint8_t head_bytes[4];
        };
    } __attribute__((packed));

    Header header_;
    uint8_t padding_[(MapSizeBytes - sizeof(Header)) % sizeof(Slot)];
    static constexpr unsigned kTotalMetadataBytes =
        sizeof(Header) + sizeof(padding_);
    union {
        // Grows from front
        Slot slot_[(MapSizeBytes - kTotalMetadataBytes) / sizeof(Slot)];
        // Grows from back
        uint8_t heap_[MapSizeBytes - kTotalMetadataBytes];
    };

    // For convenience we want the page payload size to be divisible by the Slot
    // size. This assertion ensures we are using an appropriate amount padding
    // in the header.
    static_assert((MapSizeBytes - kTotalMetadataBytes) % sizeof(Slot) == 0);

    // Maps cannot be smaller than the header (plus any needed padding).
    static_assert(MapSizeBytes > kTotalMetadataBytes,
                  "The PackedMap must be large enough to store its header.");

    uint8_t* Ptr();
    const uint8_t* Ptr() const;
    const uint8_t* GetPrefix() const;

    bool HandleDuplicateInsertion(uint16_t slot_id, const uint8_t* key,
                                  unsigned key_length, const uint8_t* payload,
                                  unsigned payload_length);
    void StoreKeyValue(uint16_t slot_id, const uint8_t* key, unsigned key_length,
                       const uint8_t* payload, unsigned payload_length);
    bool RemoveSlot(unsigned slot_id);

    unsigned SpaceNeeded(unsigned key_length, unsigned payload_length) const;
    bool RequestSpaceFor(unsigned space_needed);
    unsigned FreeSpace() const;
    unsigned FreeSpaceAfterCompaction() const;
    void Compactify();
    void RemoveFromHeapAndCompact(unsigned slot_id);
    void CopyKeyValueRange(PackedMap<MapSizeBytes>& dst, uint16_t dst_slot,
                           uint16_t src_slot, unsigned src_count);
    void CopyKeyValue(uint16_t src_slot, PackedMap<MapSizeBytes>& dst,
                      uint16_t dst_slot);

    void SetFences(const uint8_t* lower_key, unsigned lower_length,
                   const uint8_t* upper_key, unsigned upper_length);
    void InsertFence(typename Header::FenceKeySlot& fk, const uint8_t* key,
                     unsigned key_length);

    void MakeHint();
    void SearchHint(uint32_t key_head, unsigned& lower_out,
                    unsigned& upper_out) const;
    void UpdateHint(unsigned slot_id);
    unsigned LowerBound(const uint8_t* key, unsigned key_length,
                        bool& found_out) const;

    uint8_t* GetKey(unsigned slot_id);
    uint8_t* GetPayload(unsigned slot_id);
    const uint8_t* GetKey(unsigned slot_id) const;
    const uint8_t* GetPayload(unsigned slot_id) const;

    template <class T>
    static T LoadUnaligned(const void* p) {
        T x;
        memcpy(&x, p, sizeof(T));
        return x;
    }
    static uint32_t ExtractHead(const uint8_t* key, unsigned key_length) {
        switch (key_length) {
        case 0:
            return 0;
        case 1:
            return static_cast<uint32_t>(key[0]) << 24;
        case 2:
            return static_cast<uint32_t>(
                       __builtin_bswap16(LoadUnaligned<uint16_t>(key)))
                   << 16;
        case 3:
            return (static_cast<uint32_t>(
                        __builtin_bswap16(LoadUnaligned<uint16_t>(key)))
                    << 16) |
                   (static_cast<uint32_t>(key[2]) << 8);
        default:
            return __builtin_bswap32(LoadUnaligned<uint32_t>(key));
        }
    }

public:
    static constexpr size_t kMaxKeySizeBytes =
        (MapSizeBytes - kTotalMetadataBytes - (2 * sizeof(Slot))) / 4;
    static constexpr size_t kUsableSize = MapSizeBytes - kTotalMetadataBytes;
    static constexpr size_t kSlotSize = sizeof(Slot);
};



namespace detail {

static unsigned Min(unsigned a, unsigned b) {
    return a < b ? a : b;
}

}

template <uint16_t MapSizeBytes>
PackedMap<MapSizeBytes>::PackedMap(const uint8_t* lower_key,
                                   unsigned lower_key_length,
                                   const uint8_t* upper_key,
                                   unsigned upper_key_length) {
    // Verify that `lower_key` < `upper_key`. Having `lower_key` == `upper_key` is
    // usually an error (cannot store any other keys).
    const int cmp =
        memcmp(lower_key, upper_key,
               detail::Min(lower_key_length, upper_key_length));
    assert(cmp < 0 || (cmp == 0 && lower_key_length < upper_key_length) ||
           upper_key_length == 0);

    SetFences(lower_key, lower_key_length, upper_key, upper_key_length);
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::Insert(const uint8_t* key, unsigned key_length,
                                     const uint8_t* payload,
                                     unsigned payload_length) {
    bool found;
    const unsigned slot_id = LowerBound(key, key_length, found);

    // Duplicate insertion; need to check the payload length.
    if (found)
        return HandleDuplicateInsertion(slot_id, key, key_length, payload,
                                        payload_length);

    // Genuine insertion.
    if (!RequestSpaceFor(SpaceNeeded(key_length, payload_length)))
        return false;  // no space, insert fails

    memmove(slot_ + slot_id + 1, slot_ + slot_id,
            sizeof(Slot) * (header_.count - slot_id));
    StoreKeyValue(slot_id, key, key_length, payload, payload_length);
    ++header_.count;
    UpdateHint(slot_id);
    return true;
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::UpdateOrRemove(const uint8_t* key,
        unsigned key_length,
        const uint8_t* payload,
        unsigned payload_length) {
    bool found;
    const unsigned slot_id = LowerBound(key, key_length, found);

    if (!found) return false;  // Key not in the map.

    // If new payload doesn't fit, just delete the old one.
    if (HandleDuplicateInsertion(slot_id, key, key_length, payload,
                                 payload_length)) {
        return true;
    } else {
        RemoveSlot(slot_id);
        return false;
    }
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::HandleDuplicateInsertion(
    uint16_t slot_id, const uint8_t* key, unsigned key_length,
    const uint8_t* payload, unsigned payload_length) {
    int length_diff = payload_length - slot_[slot_id].payload_length;
    if (length_diff <= 0) {
        memcpy(GetPayload(slot_id), payload, payload_length);
        // If the payloads are the same length, this is a no-op. Otherwise it
        // adjusts the space used correctly.
        slot_[slot_id].payload_length = payload_length;
        header_.space_used -= static_cast<uint16_t>(-length_diff);
        return true;
    }

    // The new payload is longer than the existing payload. Can we just store
    // the key and payload again to avoid compaction?
    if (FreeSpace() >= (SpaceNeeded(key_length, payload_length) - sizeof(Slot))) {
        header_.space_used -= slot_[slot_id].key_length;
        header_.space_used -= slot_[slot_id].payload_length;
        StoreKeyValue(slot_id, key, key_length, payload, payload_length);
        // No need to update the hint because the key was already in the map
        return true;
    }

    if (FreeSpaceAfterCompaction() < length_diff) {
        // Not enough free space left, even if we compact
        return false;
    }

    RemoveFromHeapAndCompact(slot_id);
    StoreKeyValue(slot_id, key, key_length, payload, payload_length);
    // No need to update the hint because the key was already in the map
    return true;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::StoreKeyValue(uint16_t slot_id,
        const uint8_t* key,
        unsigned key_length,
        const uint8_t* payload,
        unsigned payload_length) {
    // slot
    key += header_.prefix_length;
    key_length -= header_.prefix_length;
    slot_[slot_id].head = ExtractHead(key, key_length);
    slot_[slot_id].key_length = key_length;
    slot_[slot_id].payload_length = payload_length;
    // key
    const unsigned space = key_length + payload_length;
    header_.data_offset -= space;
    header_.space_used += space;
    slot_[slot_id].offset = header_.data_offset;
    assert(GetKey(slot_id) >= reinterpret_cast<const uint8_t*>(&slot_[slot_id]));
    memcpy(GetKey(slot_id), key, key_length);
    memcpy(GetPayload(slot_id), payload, payload_length);
}

template <uint16_t MapSizeBytes>
unsigned PackedMap<MapSizeBytes>::SpaceNeeded(
    const unsigned key_length, const unsigned payload_length) const {
    assert(key_length >= header_.prefix_length);
    return sizeof(Slot) + (key_length - header_.prefix_length) + payload_length;
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::RequestSpaceFor(const unsigned space_needed) {
    if (space_needed <= FreeSpace()) return true;
    if (space_needed <= FreeSpaceAfterCompaction()) {
        Compactify();
        return true;
    }
    return false;
}

template <uint16_t MapSizeBytes>
unsigned PackedMap<MapSizeBytes>::FreeSpace() const {
    return header_.data_offset -
           (reinterpret_cast<const uint8_t*>(slot_ + header_.count) - Ptr());
}

template <uint16_t MapSizeBytes>
unsigned PackedMap<MapSizeBytes>::FreeSpaceAfterCompaction() const {
    return MapSizeBytes -
           (reinterpret_cast<const uint8_t*>(slot_ + header_.count) - Ptr()) -
           header_.space_used;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::Compactify() {
    unsigned should = FreeSpaceAfterCompaction();
    static_cast<void>(should);
    PackedMap<MapSizeBytes> tmp;
    tmp.SetFences(GetLowerFence(), header_.lower_fence.length, GetUpperFence(),
                  header_.upper_fence.length);
    CopyKeyValueRange(tmp, 0, 0, header_.count);
    memcpy(reinterpret_cast<char*>(this), &tmp, sizeof(PackedMap<MapSizeBytes>));
    MakeHint();
    assert(FreeSpace() == should);
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::SetFences(const uint8_t* lower_key,
                                        unsigned lower_length,
                                        const uint8_t* upper_key,
                                        unsigned upper_length) {
    InsertFence(header_.lower_fence, lower_key, lower_length);
    InsertFence(header_.upper_fence, upper_key, upper_length);
    for (header_.prefix_length = 0;
            (header_.prefix_length <
             detail::Min(lower_length, upper_length)) &&
            (lower_key[header_.prefix_length] == upper_key[header_.prefix_length]);
            ++header_.prefix_length)
        ;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::InsertFence(typename Header::FenceKeySlot& fk,
        const uint8_t* key,
        unsigned key_length) {
    assert(FreeSpace() >= key_length);
    header_.data_offset -= key_length;
    header_.space_used += key_length;
    fk.offset = header_.data_offset;
    fk.length = key_length;
    memcpy(Ptr() + header_.data_offset, key, key_length);
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::Append(const uint8_t* key, unsigned key_length,
                                     const uint8_t* payload,
                                     unsigned payload_length,
                                     bool perform_checks) {
    if (!RequestSpaceFor(SpaceNeeded(key_length, payload_length)))
        return false;  // no space, append fails

    // If the `perform_checks` flag is set, ensure the sorted order is not
    // violated.
    if (perform_checks) {
        int8_t check = CanAppend(key, key_length);
        if (check == 0) {  // Duplicate insertion.
            return HandleDuplicateInsertion(header_.count - 1, key, key_length,
                                            payload, payload_length);
        } else if (check < 0) {
            return Insert(key, key_length, payload, payload_length);
        }
    }

    StoreKeyValue(header_.count, key, key_length, payload, payload_length);
    UpdateHint(header_.count);
    ++header_.count;
    return true;
}

template <uint16_t MapSizeBytes>
int8_t PackedMap<MapSizeBytes>::CanAppend(const uint8_t* key,
        unsigned key_length) {
    if (header_.count == 0) return 1;  // If empty, can append.

    const uint8_t* trunc_key = key + header_.prefix_length;
    const unsigned trunc_key_length = key_length - header_.prefix_length;
    const uint8_t* top_key = GetKey(header_.count - 1);
    const unsigned top_key_length = slot_[header_.count - 1].key_length;

    int cmp = memcmp(trunc_key, top_key,
                     detail::Min(trunc_key_length, top_key_length));

    if (cmp == 0 && trunc_key_length == top_key_length) {  // Duplicate insertion.
        return 0;
    } else if ((cmp < 0) || (cmp == 0 && trunc_key_length <
                             top_key_length)) {  // Can't append.
        return -1;
    } else {  // Can append.
        return 1;
    }
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::Remove(const uint8_t* key, unsigned key_length) {
    bool found;
    const unsigned slot_id = LowerBound(key, key_length, found);
    if (!found) return false;
    return RemoveSlot(slot_id);
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::RemoveSlot(const unsigned slot_id) {
    header_.space_used -= slot_[slot_id].key_length;
    header_.space_used -= slot_[slot_id].payload_length;
    memmove(slot_ + slot_id, slot_ + slot_id + 1,
            sizeof(Slot) * (header_.count - slot_id - 1));
    header_.count--;
    MakeHint();
    return true;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::MakeHint() {
    const unsigned dist = header_.count / (kHintCount + 1);
    for (unsigned i = 0; i < kHintCount; ++i)
        header_.hint[i] = slot_[dist * (i + 1)].head;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::UpdateHint(const unsigned slot_id) {
    const unsigned dist = header_.count / (kHintCount + 1);
    unsigned begin = 0;
    if ((header_.count > kHintCount * 2 + 1) &&
            (((header_.count - 1) / (kHintCount + 1)) == dist) &&
            ((slot_id / dist) > 1))
        begin = (slot_id / dist) - 1;
    for (unsigned i = begin; i < kHintCount; ++i)
        header_.hint[i] = slot_[dist * (i + 1)].head;
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::Get(const uint8_t* key, unsigned key_length,
                                  const uint8_t** payload_out,
                                  unsigned* payload_length_out) const {
    bool found_exact = false;
    *payload_out = nullptr;
    *payload_length_out = 0;

    unsigned slot_id = LowerBound(key, key_length, found_exact);
    if (!found_exact) {
        return false;
    }

    *payload_out = GetPayload(slot_id);
    *payload_length_out = slot_[slot_id].payload_length;
    return true;
}

template <uint16_t MapSizeBytes>
unsigned PackedMap<MapSizeBytes>::LowerBound(const uint8_t* key,
        unsigned key_length,
        bool& found_out) const {
    found_out = false;

    // check prefix
    int cmp = memcmp(key, GetPrefix(),
                     detail::Min(key_length, header_.prefix_length));
    if (cmp < 0)  // key is less than prefix
        return 0;
    if (cmp > 0)  // key is greater than prefix
        return header_.count;
    if (key_length <
            header_.prefix_length)  // key is equal but shorter than prefix
        return 0;
    key += header_.prefix_length;
    key_length -= header_.prefix_length;

    // check hint
    unsigned lower = 0;
    unsigned upper = header_.count;
    uint32_t key_head = ExtractHead(key, key_length);
    SearchHint(key_head, lower, upper);

    // binary search on remaining range
    while (lower < upper) {
        unsigned mid = ((upper - lower) / 2) + lower;
        if (key_head < slot_[mid].head) {
            upper = mid;
        } else if (key_head > slot_[mid].head) {
            lower = mid + 1;
        } else {  // Head is equal, check full key
            int cmp =
                memcmp(key, GetKey(mid),
                       detail::Min(key_length, slot_[mid].key_length));
            if (cmp < 0) {
                upper = mid;
            } else if (cmp > 0) {
                lower = mid + 1;
            } else {
                if (key_length < slot_[mid].key_length) {  // key is shorter
                    upper = mid;
                } else if (key_length > slot_[mid].key_length) {  // key is longer
                    lower = mid + 1;
                } else {
                    found_out = true;
                    return mid;
                }
            }
        }
    }
    return lower;
}

template <uint16_t MapSizeBytes>
uint8_t* PackedMap<MapSizeBytes>::GetKey(unsigned slot_id) {
    return Ptr() + slot_[slot_id].offset;
}

template <uint16_t MapSizeBytes>
uint8_t* PackedMap<MapSizeBytes>::GetPayload(unsigned slot_id) {
    return Ptr() + slot_[slot_id].offset + slot_[slot_id].key_length;
}

template <uint16_t MapSizeBytes>
const uint8_t* PackedMap<MapSizeBytes>::GetKey(unsigned slot_id) const {
    return Ptr() + slot_[slot_id].offset;
}

template <uint16_t MapSizeBytes>
const uint8_t* PackedMap<MapSizeBytes>::GetPayload(unsigned slot_id) const {
    return Ptr() + slot_[slot_id].offset + slot_[slot_id].key_length;
}

template <uint16_t MapSizeBytes>
uint8_t* PackedMap<MapSizeBytes>::Ptr() {
    return reinterpret_cast<uint8_t*>(this);
}

template <uint16_t MapSizeBytes>
const uint8_t* PackedMap<MapSizeBytes>::Ptr() const {
    return reinterpret_cast<const uint8_t*>(this);
}

template <uint16_t MapSizeBytes>
const uint8_t* PackedMap<MapSizeBytes>::GetPrefix() const {
    return Ptr() + header_.lower_fence.offset;
}

template <uint16_t MapSizeBytes>
const uint8_t* PackedMap<MapSizeBytes>::GetLowerFence() const {
    return Ptr() + header_.lower_fence.offset;
}

template <uint16_t MapSizeBytes>
const uint8_t* PackedMap<MapSizeBytes>::GetUpperFence() const {
    return Ptr() + header_.upper_fence.offset;
}

template <uint16_t MapSizeBytes>
const uint16_t PackedMap<MapSizeBytes>::GetLowerFenceLength() const {
    return header_.lower_fence.length;
}

template <uint16_t MapSizeBytes>
const uint16_t PackedMap<MapSizeBytes>::GetUpperFenceLength() const {
    return header_.upper_fence.length;
}

template <uint16_t MapSizeBytes>
const bool PackedMap<MapSizeBytes>::IsValid() const {
    return header_.flags & kValidFlag;
}

template <uint16_t MapSizeBytes>
const bool PackedMap<MapSizeBytes>::IsOverflow() const {
    return header_.flags & kOverflowFlag;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::MakeOverflow() {
    header_.flags |= kOverflowFlag;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::UnmakeOverflow() {
    header_.flags &= ~kOverflowFlag;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::SearchHint(const uint32_t key_head,
        unsigned& lower_out,
        unsigned& upper_out) const {
    if (header_.count > kHintCount * 2) {
        unsigned dist = upper_out / (kHintCount + 1);
        unsigned pos, pos2;
        for (pos = 0; pos < kHintCount; ++pos)
            if (header_.hint[pos] >= key_head) break;
        for (pos2 = pos; pos2 < kHintCount; ++pos2)
            if (header_.hint[pos2] != key_head) break;
        lower_out = pos * dist;
        if (pos2 < kHintCount) upper_out = (pos2 + 1) * dist;
    }
}

template <uint16_t MapSizeBytes>
PhysicalPageId PackedMap<MapSizeBytes>::GetOverflow() const {
    return header_.overflow;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::SetOverflow(PhysicalPageId overflow) {
    header_.overflow = overflow;
}

template <uint16_t MapSizeBytes>
uint16_t PackedMap<MapSizeBytes>::GetNumRecords() const {
    return header_.count;
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::GetKeyPrefix(
    const uint8_t** key_prefix_out, unsigned* key_prefix_length_out) const {
    *key_prefix_out = Ptr() + header_.lower_fence.offset;
    *key_prefix_length_out = header_.prefix_length;
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::GetKeySuffixInSlot(
    const uint16_t slot_id, const uint8_t** key_suffix_out,
    unsigned* key_suffix_length_out) const {
    if (slot_id >= GetNumRecords()) {
        *key_suffix_out = nullptr;
        *key_suffix_length_out = 0;
        return false;
    }
    *key_suffix_out = GetKey(slot_id);
    *key_suffix_length_out = slot_[slot_id].key_length;
    return true;
}

template <uint16_t MapSizeBytes>
bool PackedMap<MapSizeBytes>::GetPayloadInSlot(
    const uint16_t slot_id, const uint8_t** payload_out,
    unsigned* payload_length_out) const {
    if (slot_id >= GetNumRecords()) {
        *payload_out = nullptr;
        *payload_length_out = 0;
        return false;
    }
    *payload_out = GetPayload(slot_id);
    *payload_length_out = slot_[slot_id].payload_length;
    return true;
}

template <uint16_t MapSizeBytes>
uint16_t PackedMap<MapSizeBytes>::LowerBoundSlot(const uint8_t* key,
        unsigned key_length) const {
    bool found = false;
    return LowerBound(key, key_length, found);
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::CopyKeyValueRange(PackedMap<MapSizeBytes>& dst,
        uint16_t dst_slot,
        uint16_t src_slot,
        unsigned src_count) {
    if (header_.prefix_length <= dst.header_.prefix_length) {  // prefix grows
        unsigned diff = dst.header_.prefix_length - header_.prefix_length;
        for (unsigned i = 0; i < src_count; ++i) {
            const unsigned new_key_length = slot_[src_slot + i].key_length - diff;
            const unsigned space =
                new_key_length + slot_[src_slot + i].payload_length;
            dst.header_.data_offset -= space;
            dst.header_.space_used += space;
            dst.slot_[dst_slot + i].offset = dst.header_.data_offset;
            uint8_t* key = GetKey(src_slot + i) + diff;
            memcpy(dst.GetKey(dst_slot + i), key, space);
            dst.slot_[dst_slot + i].head =
                ExtractHead(key, new_key_length);
            dst.slot_[dst_slot + i].key_length = new_key_length;
            dst.slot_[dst_slot + i].payload_length =
                slot_[src_slot + i].payload_length;
        }
    } else {
        for (unsigned i = 0; i < src_count; ++i)
            CopyKeyValue(src_slot + i, dst, dst_slot + i);
    }
    dst.header_.count += src_count;
    assert((dst.Ptr() + dst.header_.data_offset) >=
           reinterpret_cast<uint8_t*>(dst.slot_ + dst.header_.count));
}

template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::CopyKeyValue(uint16_t src_slot,
        PackedMap<MapSizeBytes>& dst,
        uint16_t dst_slot) {
    const unsigned full_length =
        slot_[src_slot].key_length + header_.prefix_length;
    uint8_t key[full_length];
    memcpy(key, GetPrefix(), header_.prefix_length);
    memcpy(key + header_.prefix_length, GetKey(src_slot),
           slot_[src_slot].key_length);
    dst.StoreKeyValue(dst_slot, key, full_length, GetPayload(src_slot),
                      slot_[src_slot].payload_length);
}

// Removes the specified slot's key and payload from the heap and then compacts
// the heap. This is useful when we need to update an existing key-payload
// record with a longer payload.
template <uint16_t MapSizeBytes>
void PackedMap<MapSizeBytes>::RemoveFromHeapAndCompact(unsigned slot_id) {
    const unsigned new_space = header_.space_used - slot_[slot_id].key_length -
                               slot_[slot_id].payload_length;
    uint8_t buf[new_space];
    uint16_t buf_offset = new_space;
    uint16_t map_offset = MapSizeBytes;

    // 1. Copy over the fences, if they exist
    typename Header::FenceKeySlot* fences[2] = {&(header_.lower_fence),
                                                &(header_.upper_fence)
                                               };
    for (auto& fence : fences) {
        if (fence->length == 0) {
            continue;
        }
        buf_offset -= fence->length;
        map_offset -= fence->length;
        assert(buf_offset <= new_space);
        assert(map_offset <= MapSizeBytes);
        memcpy(&buf[buf_offset], Ptr() + fence->offset, fence->length);
        fence->offset = map_offset;
    }

    // 2. Copy over the rest of the heap slot by slot
    for (uint16_t id = 0; id < header_.count; ++id) {
        if (id == slot_id) {
            // This slot's offset is now invalid - set it to a value that will likely
            // trigger a memory error if used as an offset inside this map.
            slot_[id].offset = UINT16_MAX;
            continue;
        }
        unsigned record_length = slot_[id].key_length + slot_[id].payload_length;
        buf_offset -= record_length;
        map_offset -= record_length;
        assert(buf_offset <= new_space);
        assert(map_offset <= MapSizeBytes);
        memcpy(&buf[buf_offset], GetKey(id), record_length);
        slot_[id].offset = map_offset;
    }
    // Sanity check: We should have filled the entire buffer
    assert(buf_offset == 0);

    // 3. Copy over the compacted heap
    header_.data_offset = map_offset;
    header_.space_used = new_space;
    memcpy(Ptr() + header_.data_offset, buf, new_space);
    assert((Ptr() + header_.data_offset) >=
           reinterpret_cast<const uint8_t*>(slot_ + header_.count));
    // We do not modify header_.count because we leave the slot unmodified
}

}
