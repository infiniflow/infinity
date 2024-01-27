module;
#include <cassert>
import stl;
import third_party;
export module fst:bytes;
import :writer;

namespace infinity {

/// Read a u32 in little endian format from the beginning of the given slice.
u32 ReadU32LE(const u8 *ptr) {
    // Most architectures have alignment requirements, and violating those requirements may result in undefined behavior. Some architectures may raise
    // alignment exceptions, while others may silently provide incorrect values. Keep in mind that this method may have some performance implications,
    // and on architectures that support unaligned access, the compiler may optimize this code to take advantage of such support.
    u32 result;
    std::memcpy(&result, ptr, sizeof(u32));
#ifdef __BIG_ENDIAN__
    return __builtin_bswap32(result);
#else
    return result;
#endif
}

/// Read a u64 in little endian format from the beginning of the given slice.
u64 ReadU64LE(const u8 *ptr) {
    u64 result;
    std::memcpy(&result, ptr, sizeof(u64));
#ifdef __BIG_ENDIAN__
    return __builtin_bswap64(*(u64 *)(ptr));
#else
    return *(u64 *)(ptr);
#endif
}

/// Write a u32 in little endian format to the beginning of the given ptr.
void WriteU32LE(u32 n, u8 *ptr) {
#ifdef __BIG_ENDIAN__
    n = __builtin_bswap32(n);
#endif
    std::memcpy(ptr, &n, sizeof(u32));
}

/// Like WriteU32LE, but to an ostream implementation.
void IoWriteU32LE(u32 n, Writer &wtr) {
#ifdef __BIG_ENDIAN__
    n = __builtin_bswap32(n);
#endif
    wtr.Write((u8 *)&n, 4);
}

/// Write a u64 in little endian format to the beginning of the given ptr.
void WriteU64LE(u64 n, u8 *ptr) {
#ifdef __BIG_ENDIAN__
    n = __builtin_bswap64(n);
#endif
    std::memcpy(ptr, &n, sizeof(u64));
}

/// Like WriteU64LE, but to an ostream implementation.
void IoWriteU64LE(u64 n, Writer &wtr) {
#ifdef __BIG_ENDIAN__
    n = __builtin_bswap64(n);
#endif
    wtr.Write((u8 *)&n, 8);
}

/// PackSize returns the smallest number of bytes that can encode `n`.
u8 PackSize(u64 n) {
    if (n < 1ULL << 8) {
        return 1;
    } else if (n < 1ULL << 16) {
        return 2;
    } else if (n < 1ULL << 24) {
        return 3;
    } else if (n < 1ULL << 32) {
        return 4;
    } else if (n < 1ULL << 40) {
        return 5;
    } else if (n < 1ULL << 48) {
        return 6;
    } else if (n < 1ULL << 56) {
        return 7;
    } else {
        return 8;
    }
}

/// PackUintIn is like PackUint, but always uses the number of bytes given
/// to pack the number given.
///
/// `nbytes` must be >= pack_size(n) and <= 8, where `pack_size(n)` is the
/// smallest number of bytes that can store the integer given.
void PackUintIn(Writer &wtr, u64 n, u8 nbytes) {
    assert(nbytes >= 1 && nbytes <= 8);
#ifdef __BIG_ENDIAN__
    n = __builtin_bswap64(n);
#endif
    wtr.Write((u8 *)&n, nbytes);
}

/// PackUint packs the given integer in the smallest number of bytes possible,
/// and writes it to the given writer. The number of bytes written is returned
/// on success.
u8 PackUint(Writer &wtr, u64 n) {
    u8 nbytes = PackSize(n);
    PackUintIn(wtr, n, nbytes);
    return nbytes;
}

/// UnpackUint is the dual of PackUint. It unpacks the integer at the current
/// position in `ptr` after reading `nbytes` bytes.
///
/// `nbytes` must be >= 1 and <= 8.
u64 UnpackUint(u8 *ptr, u8 nbytes) {
    assert(nbytes >= 1 && nbytes <= 8);
    u64 n = 0;
    std::memcpy(&n, ptr, nbytes);
#ifdef __BIG_ENDIAN__
    n = __builtin_bswap64(n);
#endif
    return n;
}

/// Compare two byte slice according to the lexicographically order
int CompareBytes(u8 *bs1_data, SizeT bs1_len, u8 *bs2_data, SizeT bs2_len) {
    SizeT common_len = std::min(bs1_len, bs2_len);
    int ret = std::memcmp(bs1_data, bs2_data, common_len);
    if (ret != 0) {
        return ret;
    }
    if (bs1_len < bs2_len) {
        return -1;
    } else if (bs1_len > bs2_len) {
        return 1;
    } else {
        return 0;
    }
}

} // namespace infinity