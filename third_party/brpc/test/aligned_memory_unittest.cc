// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "butil/memory/aligned_memory.h"
#include "butil/memory/scoped_ptr.h"
#include <gtest/gtest.h>

#define EXPECT_ALIGNED(ptr, align) \
    EXPECT_EQ(0u, reinterpret_cast<uintptr_t>(ptr) & (align - 1))

namespace {

using butil::AlignedMemory;

TEST(AlignedMemoryTest, StaticAlignment) {
  static AlignedMemory<8, 8> raw8;
  static AlignedMemory<8, 16> raw16;
  static AlignedMemory<8, 256> raw256;
  static AlignedMemory<8, 4096> raw4096;

  EXPECT_EQ(8u, ALIGNOF(raw8));
  EXPECT_EQ(16u, ALIGNOF(raw16));
  EXPECT_EQ(256u, ALIGNOF(raw256));
  EXPECT_EQ(4096u, ALIGNOF(raw4096));

  EXPECT_ALIGNED(raw8.void_data(), 8);
  EXPECT_ALIGNED(raw16.void_data(), 16);
  EXPECT_ALIGNED(raw256.void_data(), 256);
  EXPECT_ALIGNED(raw4096.void_data(), 4096);
}

// stack alignment is buggy before gcc 4.6
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=16660
#if defined(COMPILER_GCC) &&                                    \
    ( __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#define GOOD_GCC_STACK_ALIGNMENT
#endif

TEST(AlignedMemoryTest, StackAlignment) {
  AlignedMemory<8, 8> raw8;
  AlignedMemory<8, 16> raw16;
  AlignedMemory<8, 128> raw128;

  EXPECT_EQ(8u, ALIGNOF(raw8));
  EXPECT_EQ(16u, ALIGNOF(raw16));
  EXPECT_EQ(128u, ALIGNOF(raw128));

  EXPECT_ALIGNED(raw8.void_data(), 8);
  EXPECT_ALIGNED(raw16.void_data(), 16);

  // TODO(ios): __attribute__((aligned(X))) with X >= 128 does not works on
  // the stack when building for arm64 on iOS, http://crbug.com/349003
#if !(defined(OS_IOS) && defined(ARCH_CPU_ARM64)) &&    \
    defined(GOOD_GCC_STACK_ALIGNMENT)
  EXPECT_ALIGNED(raw128.void_data(), 128);

  // NaCl x86-64 compiler emits non-validating instructions for >128
  // bytes alignment.
  // http://www.chromium.org/nativeclient/design-documents/nacl-sfi-model-on-x86-64-systems
  // TODO(hamaji): Ideally, NaCl compiler for x86-64 should workaround
  // this limitation and this #if should be removed.
  // https://code.google.com/p/nativeclient/issues/detail?id=3463
#if !(defined(OS_NACL) && defined(ARCH_CPU_X86_64)) &&  \
    defined(GOOD_GCC_STACK_ALIGNMENT)
  AlignedMemory<8, 256> raw256;
  EXPECT_EQ(256u, ALIGNOF(raw256));
  EXPECT_ALIGNED(raw256.void_data(), 256);

  // TODO(ios): This test hits an armv7 bug in clang. crbug.com/138066
#if !(defined(OS_IOS) && defined(ARCH_CPU_ARM_FAMILY)) &&       \
    defined(GOOD_GCC_STACK_ALIGNMENT)
  AlignedMemory<8, 4096> raw4096;
  EXPECT_EQ(4096u, ALIGNOF(raw4096));
  EXPECT_ALIGNED(raw4096.void_data(), 4096);
#endif  // !(defined(OS_IOS) && defined(ARCH_CPU_ARM_FAMILY))
#endif  // !(defined(OS_NACL) && defined(ARCH_CPU_X86_64))
#endif  // !(defined(OS_IOS) && defined(ARCH_CPU_ARM64))
}

TEST(AlignedMemoryTest, DynamicAllocation) {
  void* p = butil::AlignedAlloc(8, 8);
  EXPECT_TRUE(p);
  EXPECT_ALIGNED(p, 8);
  butil::AlignedFree(p);

  p = butil::AlignedAlloc(8, 16);
  EXPECT_TRUE(p);
  EXPECT_ALIGNED(p, 16);
  butil::AlignedFree(p);

  p = butil::AlignedAlloc(8, 256);
  EXPECT_TRUE(p);
  EXPECT_ALIGNED(p, 256);
  butil::AlignedFree(p);

  p = butil::AlignedAlloc(8, 4096);
  EXPECT_TRUE(p);
  EXPECT_ALIGNED(p, 4096);
  butil::AlignedFree(p);
}

TEST(AlignedMemoryTest, ScopedDynamicAllocation) {
  scoped_ptr<float, butil::AlignedFreeDeleter> p(
      static_cast<float*>(butil::AlignedAlloc(8, 8)));
  EXPECT_TRUE(p.get());
  EXPECT_ALIGNED(p.get(), 8);
}

}  // namespace
