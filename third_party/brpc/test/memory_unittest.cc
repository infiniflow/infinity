// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#define _CRT_SECURE_NO_WARNINGS

#include "butil/process/memory.h"

#include <limits>

#include "butil/compiler_specific.h"
#include "butil/debug/alias.h"
#include "butil/strings/stringprintf.h"
#include <gtest/gtest.h>

#if defined(OS_WIN)
#include <windows.h>
#endif
#if defined(OS_POSIX)
#include <errno.h>
#endif
#if defined(OS_MACOSX)
#include <malloc/malloc.h>
#include "butil/process/memory_unittest_mac.h"
#endif
#if defined(OS_LINUX)
#include <malloc.h>
#endif

#if defined(OS_WIN)
// HeapQueryInformation function pointer.
typedef BOOL (WINAPI* HeapQueryFn)  \
    (HANDLE, HEAP_INFORMATION_CLASS, PVOID, SIZE_T, PSIZE_T);

const int kConstantInModule = 42;

TEST(ProcessMemoryTest, GetModuleFromAddress) {
  // Since the unit tests are their own EXE, this should be
  // equivalent to the EXE's HINSTANCE.
  //
  // kConstantInModule is a constant in this file and
  // therefore within the unit test EXE.
  EXPECT_EQ(::GetModuleHandle(NULL),
            butil::GetModuleFromAddress(
                const_cast<int*>(&kConstantInModule)));

  // Any address within the kernel32 module should return
  // kernel32's HMODULE.  Our only assumption here is that
  // kernel32 is larger than 4 bytes.
  HMODULE kernel32 = ::GetModuleHandle(L"kernel32.dll");
  HMODULE kernel32_from_address =
      butil::GetModuleFromAddress(reinterpret_cast<DWORD*>(kernel32) + 1);
  EXPECT_EQ(kernel32, kernel32_from_address);
}

TEST(ProcessMemoryTest, EnableLFH) {
  ASSERT_TRUE(butil::EnableLowFragmentationHeap());
  if (IsDebuggerPresent()) {
    // Under these conditions, LFH can't be enabled. There's no point to test
    // anything.
    const char* no_debug_env = getenv("_NO_DEBUG_HEAP");
    if (!no_debug_env || strcmp(no_debug_env, "1"))
      return;
  }
  HMODULE kernel32 = GetModuleHandle(L"kernel32.dll");
  ASSERT_TRUE(kernel32 != NULL);
  HeapQueryFn heap_query = reinterpret_cast<HeapQueryFn>(GetProcAddress(
      kernel32,
      "HeapQueryInformation"));

  // On Windows 2000, the function is not exported. This is not a reason to
  // fail but we won't be able to retrieves information about the heap, so we
  // should stop here.
  if (heap_query == NULL)
    return;

  HANDLE heaps[1024] = { 0 };
  unsigned number_heaps = GetProcessHeaps(1024, heaps);
  EXPECT_GT(number_heaps, 0u);
  for (unsigned i = 0; i < number_heaps; ++i) {
    ULONG flag = 0;
    SIZE_T length;
    ASSERT_NE(0, heap_query(heaps[i],
                            HeapCompatibilityInformation,
                            &flag,
                            sizeof(flag),
                            &length));
    // If flag is 0, the heap is a standard heap that does not support
    // look-asides. If flag is 1, the heap supports look-asides. If flag is 2,
    // the heap is a low-fragmentation heap (LFH). Note that look-asides are not
    // supported on the LFH.

    // We don't have any documented way of querying the HEAP_NO_SERIALIZE flag.
    EXPECT_LE(flag, 2u);
    EXPECT_NE(flag, 1u);
  }
}
#endif  // defined(OS_WIN)

#if defined(OS_MACOSX)

// For the following Mac tests:
// Note that butil::EnableTerminationOnHeapCorruption() is called as part of
// test suite setup and does not need to be done again, else mach_override
// will fail.

#if !defined(ADDRESS_SANITIZER)
// The following code tests the system implementation of malloc() thus no need
// to test it under AddressSanitizer.
TEST(ProcessMemoryTest, MacMallocFailureDoesNotTerminate) {
  // Test that ENOMEM doesn't crash via CrMallocErrorBreak two ways: the exit
  // code and lack of the error string. The number of bytes is one less than
  // MALLOC_ABSOLUTE_MAX_SIZE, more than which the system early-returns NULL and
  // does not call through malloc_error_break(). See the comment at
  // EnableTerminationOnOutOfMemory() for more information.
  void* buf = NULL;
  ASSERT_EXIT(
      {
        butil::EnableTerminationOnOutOfMemory();

        buf = malloc(std::numeric_limits<size_t>::max() - (2 * PAGE_SIZE) - 1);
      },
      testing::KilledBySignal(SIGTRAP),
      "\\*\\*\\* error: can't allocate region.*\\n?.*");

  butil::debug::Alias(buf);
}
#endif  // !defined(ADDRESS_SANITIZER)

TEST(ProcessMemoryTest, MacTerminateOnHeapCorruption) {
  // Assert that freeing an unallocated pointer will crash the process.
  char buf[9];
  asm("" : "=r" (buf));  // Prevent clang from being too smart.
#if ARCH_CPU_64_BITS
  // On 64 bit Macs, the malloc system automatically abort()s on heap corruption
  // but does not output anything.
  ASSERT_DEATH(free(buf), "");
#elif defined(ADDRESS_SANITIZER)
  // AddressSanitizer replaces malloc() and prints a different error message on
  // heap corruption.
  ASSERT_DEATH(free(buf), "attempting free on address which "
      "was not malloc\\(\\)-ed");
#else
  ASSERT_DEATH(free(buf), "being freed.*\\n?\\.*"
      "\\*\\*\\* set a breakpoint in malloc_error_break to debug.*\\n?.*"
      "Terminating process due to a potential for future heap corruption");
#endif  // ARCH_CPU_64_BITS || defined(ADDRESS_SANITIZER)
}

#endif  // defined(OS_MACOSX)

// Android doesn't implement set_new_handler, so we can't use the
// OutOfMemoryTest cases.
// OpenBSD does not support these tests either.
// TODO(vandebo) make this work on Windows too.
#if !defined(OS_ANDROID) && !defined(OS_OPENBSD) && \
    !defined(OS_WIN)

#if defined(USE_TCMALLOC)
extern "C" {
int tc_set_new_mode(int mode);
}
#endif  // defined(USE_TCMALLOC)

class OutOfMemoryTest : public testing::Test {
 public:
  OutOfMemoryTest()
    : value_(NULL),
    // Make test size as large as possible minus a few pages so
    // that alignment or other rounding doesn't make it wrap.
    test_size_(std::numeric_limits<std::size_t>::max() - 12 * 1024),
    signed_test_size_(std::numeric_limits<ssize_t>::max()) {
  }

#if defined(USE_TCMALLOC)
  virtual void SetUp() OVERRIDE {
    tc_set_new_mode(1);
  }

  virtual void TearDown() OVERRIDE {
    tc_set_new_mode(0);
  }
#endif  // defined(USE_TCMALLOC)

 protected:
  void* value_;
  size_t test_size_;
  ssize_t signed_test_size_;
};

class OutOfMemoryDeathTest : public OutOfMemoryTest {
 public:
  void SetUpInDeathAssert() {
    // Must call EnableTerminationOnOutOfMemory() because that is called from
    // chrome's main function and therefore hasn't been called yet.
    // Since this call may result in another thread being created and death
    // tests shouldn't be started in a multithread environment, this call
    // should be done inside of the ASSERT_DEATH.
    butil::EnableTerminationOnOutOfMemory();
  }
};

TEST_F(OutOfMemoryDeathTest, New) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = operator new(test_size_);
    }, "");
}

TEST_F(OutOfMemoryDeathTest, NewArray) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = new char[test_size_];
    }, "");
}

TEST_F(OutOfMemoryDeathTest, Malloc) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = malloc(test_size_);
    }, "");
}

TEST_F(OutOfMemoryDeathTest, Realloc) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = realloc(NULL, test_size_);
    }, "");
}

TEST_F(OutOfMemoryDeathTest, Calloc) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = calloc(1024, test_size_ / 1024L);
    }, "");
}

TEST_F(OutOfMemoryDeathTest, Valloc) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = valloc(test_size_);
    }, "");
}

#if defined(OS_LINUX)

// pvalloc does not crash in gcc 3.4
#if PVALLOC_AVAILABLE == 1 && (!defined(__GNUC__) || __GNUC__ >= 4)
TEST_F(OutOfMemoryDeathTest, Pvalloc) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = pvalloc(test_size_);
    }, "");
}
#endif  // PVALLOC_AVAILABLE == 1

TEST_F(OutOfMemoryDeathTest, Memalign) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = memalign(4, test_size_);
    }, "");
}

// FIXME(gejun): This case fails right now, maybe related to not linking tcmalloc
/*
TEST_F(OutOfMemoryDeathTest, ViaSharedLibraries) {
  // This tests that the run-time symbol resolution is overriding malloc for
  // shared libraries (including libc itself) as well as for our code.
  std::string format = butil::StringPrintf("%%%zud", test_size_);
  char *value = NULL;
  ASSERT_DEATH({
      SetUpInDeathAssert();
      EXPECT_EQ(-1, asprintf(&value, format.c_str(), 0));
    }, "");
}
*/

#endif  // OS_LINUX

// Android doesn't implement posix_memalign().
#if defined(OS_POSIX) && !defined(OS_ANDROID)
TEST_F(OutOfMemoryDeathTest, Posix_memalign) {
  // Grab the return value of posix_memalign to silence a compiler warning
  // about unused return values. We don't actually care about the return
  // value, since we're asserting death.
  ASSERT_DEATH({
      SetUpInDeathAssert();
      EXPECT_EQ(ENOMEM, posix_memalign(&value_, 8, test_size_));
    }, "");
}
#endif  // defined(OS_POSIX) && !defined(OS_ANDROID)

#if defined(OS_MACOSX)

// Purgeable zone tests

TEST_F(OutOfMemoryDeathTest, MallocPurgeable) {
  malloc_zone_t* zone = malloc_default_purgeable_zone();
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = malloc_zone_malloc(zone, test_size_);
    }, "");
}

TEST_F(OutOfMemoryDeathTest, ReallocPurgeable) {
  malloc_zone_t* zone = malloc_default_purgeable_zone();
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = malloc_zone_realloc(zone, NULL, test_size_);
    }, "");
}

TEST_F(OutOfMemoryDeathTest, CallocPurgeable) {
  malloc_zone_t* zone = malloc_default_purgeable_zone();
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = malloc_zone_calloc(zone, 1024, test_size_ / 1024L);
    }, "");
}

TEST_F(OutOfMemoryDeathTest, VallocPurgeable) {
  malloc_zone_t* zone = malloc_default_purgeable_zone();
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = malloc_zone_valloc(zone, test_size_);
    }, "");
}

TEST_F(OutOfMemoryDeathTest, PosixMemalignPurgeable) {
  malloc_zone_t* zone = malloc_default_purgeable_zone();
  ASSERT_DEATH({
      SetUpInDeathAssert();
      value_ = malloc_zone_memalign(zone, 8, test_size_);
    }, "");
}

// Since these allocation functions take a signed size, it's possible that
// calling them just once won't be enough to exhaust memory. In the 32-bit
// environment, it's likely that these allocation attempts will fail because
// not enough contiguous address space is available. In the 64-bit environment,
// it's likely that they'll fail because they would require a preposterous
// amount of (virtual) memory.

TEST_F(OutOfMemoryDeathTest, CFAllocatorSystemDefault) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      while ((value_ =
              butil::AllocateViaCFAllocatorSystemDefault(signed_test_size_))) {}
    }, "");
}

TEST_F(OutOfMemoryDeathTest, CFAllocatorMalloc) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      while ((value_ =
              butil::AllocateViaCFAllocatorMalloc(signed_test_size_))) {}
    }, "");
}

TEST_F(OutOfMemoryDeathTest, CFAllocatorMallocZone) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      while ((value_ =
              butil::AllocateViaCFAllocatorMallocZone(signed_test_size_))) {}
    }, "");
}

#if !defined(ARCH_CPU_64_BITS)

// See process_util_unittest_mac.mm for an explanation of why this test isn't
// run in the 64-bit environment.

TEST_F(OutOfMemoryDeathTest, PsychoticallyBigObjCObject) {
  ASSERT_DEATH({
      SetUpInDeathAssert();
      while ((value_ = butil::AllocatePsychoticallyBigObjCObject())) {}
    }, "");
}

#endif  // !ARCH_CPU_64_BITS
#endif  // OS_MACOSX

class OutOfMemoryHandledTest : public OutOfMemoryTest {
 public:
  static const size_t kSafeMallocSize = 512;
  static const size_t kSafeCallocSize = 128;
  static const size_t kSafeCallocItems = 4;

  virtual void SetUp() {
    OutOfMemoryTest::SetUp();

    // We enable termination on OOM - just as Chrome does at early
    // initialization - and test that UncheckedMalloc and  UncheckedCalloc
    // properly by-pass this in order to allow the caller to handle OOM.
    butil::EnableTerminationOnOutOfMemory();
  }
};

// TODO(b.kelemen): make UncheckedMalloc and UncheckedCalloc work
// on Windows as well.
// UncheckedMalloc() and UncheckedCalloc() work as regular malloc()/calloc()
// under sanitizer tools.
#if !defined(MEMORY_TOOL_REPLACES_ALLOCATOR)
TEST_F(OutOfMemoryHandledTest, UncheckedMalloc) {
  EXPECT_TRUE(butil::UncheckedMalloc(kSafeMallocSize, &value_));
  EXPECT_TRUE(value_ != NULL);
  free(value_);

  EXPECT_FALSE(butil::UncheckedMalloc(test_size_, &value_));
  EXPECT_TRUE(value_ == NULL);
}

TEST_F(OutOfMemoryHandledTest, UncheckedCalloc) {
  EXPECT_TRUE(butil::UncheckedCalloc(1, kSafeMallocSize, &value_));
  EXPECT_TRUE(value_ != NULL);
  const char* bytes = static_cast<const char*>(value_);
  for (size_t i = 0; i < kSafeMallocSize; ++i)
    EXPECT_EQ(0, bytes[i]);
  free(value_);

  EXPECT_TRUE(
      butil::UncheckedCalloc(kSafeCallocItems, kSafeCallocSize, &value_));
  EXPECT_TRUE(value_ != NULL);
  bytes = static_cast<const char*>(value_);
  for (size_t i = 0; i < (kSafeCallocItems * kSafeCallocSize); ++i)
    EXPECT_EQ(0, bytes[i]);
  free(value_);

  EXPECT_FALSE(butil::UncheckedCalloc(1, test_size_, &value_));
  EXPECT_TRUE(value_ == NULL);
}
#endif  // !defined(MEMORY_TOOL_REPLACES_ALLOCATOR)
#endif  // !defined(OS_ANDROID) && !defined(OS_OPENBSD) && !defined(OS_WIN)
