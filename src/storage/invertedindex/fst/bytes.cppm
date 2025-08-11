// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <cassert>

export module infinity_core:fst.bytes;

import :stl;
import third_party;
import :fst.writer;

namespace infinity {

/// Read a u32 in little endian format from the beginning of the given slice.
/// Refers to https://www.kernel.org/doc/html/latest/core-api/unaligned-memory-access.html
u32 ReadU32LE(const u8 *ptr) {
#ifdef HAVE_EFFICIENT_UNALIGNED_ACCESS
    u32 result = *(u32 *)ptr;
#else
    u32 result = ((u32)ptr[0]) | ((u32)ptr[1] << 8) | ((u32)ptr[2] << 16) | ((u32)ptr[3] << 24);
#endif
#ifdef __BIG_ENDIAN__
    return __builtin_bswap32(result);
#else
    return result;
#endif
}

/// Read a u64 in little endian format from the beginning of the given slice.
export u64 ReadU64LE(const u8 *ptr) {
#ifdef HAVE_EFFICIENT_UNALIGNED_ACCESS
    u64 result = *(u64 *)ptr;
#else
    u64 result = ((u64)ptr[0]) | ((u64)ptr[1] << 8) | ((u64)ptr[2] << 16) | ((u64)ptr[3] << 24) | ((u64)ptr[4] << 32) | ((u64)ptr[5] << 40) |
                 ((u64)ptr[6] << 48) | ((u64)ptr[7] << 56);
#endif
#ifdef __BIG_ENDIAN__
    return __builtin_bswap64(result);
#else
    return result;
#endif
}

/// Write a u32 in little endian format to the beginning of the given ptr.
void WriteU32LE(u32 n, u8 *ptr) {
#ifdef __BIG_ENDIAN__
    n = __builtin_bswap32(n);
#endif
#ifdef HAVE_EFFICIENT_UNALIGNED_ACCESS
    *(u32 *)ptr = n;
#else
    ptr[0] = u8(n);
    ptr[1] = u8(n >> 8);
    ptr[2] = u8(n >> 16);
    ptr[3] = u8(n >> 24);
#endif
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
#ifdef HAVE_EFFICIENT_UNALIGNED_ACCESS
    *(u64 *)ptr = n;
#else
    ptr[0] = u8(n);
    ptr[1] = u8(n >> 8);
    ptr[2] = u8(n >> 16);
    ptr[3] = u8(n >> 24);
    ptr[4] = u8(n >> 32);
    ptr[5] = u8(n >> 40);
    ptr[6] = u8(n >> 48);
    ptr[7] = u8(n >> 56);
#endif
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
    // __builtin_clzl is a bit slower than cascaded if:
    // return n==0 ? 1 : 8 - (__builtin_clzl(n) >> 3);
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
#ifdef HAVE_EFFICIENT_UNALIGNED_ACCESS
    static const u64 masks[] = {
        0x0000000000000000,
        0x00000000000000FF,
        0x000000000000FFFF,
        0x0000000000FFFFFF,
        0x00000000FFFFFFFF,
        0x000000FFFFFFFFFF,
        0x0000FFFFFFFFFFFF,
        0x00FFFFFFFFFFFFFF,
        0xFFFFFFFFFFFFFFFF,
    };
    u64 n = *(u64 *)ptr;
    n &= masks[nbytes];
#else
    u64 n = 0;
    for (u8 i = 0; i < nbytes; i++) {
        n = n | ((u64)ptr[i]) << (8 * i);
    }
#endif
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