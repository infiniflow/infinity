// Copyright (c) 2020, Sergey Abbakumov
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "rst/macros/thread_annotations.h"

#include <memory>
#include <mutex>

#include <gtest/gtest.h>

#include "rst/not_null/not_null.h"

namespace rst {
namespace {

class AnnotatedClass {
 public:
  int ReadInt() {
    std::lock_guard lock(mtx_);
    return i_;
  }

  void WriteInt(const int i) {
    std::lock_guard lock(mtx_);
    i_ = i;
  }

  int ReadFromPointer() {
    std::lock_guard lock(mtx_);
    return *p_;
  }

  void WriteToPointer(const int i) {
    std::lock_guard lock(mtx_);
    *p_ = i;
  }

  int ReadFromUniquePointer() {
    std::lock_guard lock(mtx_);
    return *unique_p_;
  }

  void WriteToUniquePointer(const int i) {
    std::lock_guard lock(mtx_);
    *unique_p_ = i;
  }

  int ReadFromSharedPointer() {
    std::lock_guard lock(mtx_);
    return *shared_p_;
  }

  void WriteToSharedPointer(const int i) {
    std::lock_guard lock(mtx_);
    *shared_p_ = i;
  }

  int ReadFromNotNullPointer() {
    std::lock_guard lock(mtx_);
    return *not_null_p_;
  }

  void WriteToNotNullPointer(const int i) {
    std::lock_guard lock(mtx_);
    *not_null_p_ = i;
  }

  int ReadFromNullablePointer() {
    std::lock_guard lock(mtx_);
    return *nullable_p_;
  }

  void WriteToNullablePointer(const int i) {
    std::lock_guard lock(mtx_);
    *nullable_p_ = i;
  }

  int ReadFromNotNullUniquePointer() {
    std::lock_guard lock(mtx_);
    return *not_null_unique_p_;
  }

  void WriteToNotNullUniquePointer(const int i) {
    std::lock_guard lock(mtx_);
    *not_null_unique_p_ = i;
  }

  int ReadFromNullableUniquePointer() {
    std::lock_guard lock(mtx_);
    return *nullable_unique_p_;
  }

  void WriteToNullableUniquePointer(const int i) {
    std::lock_guard lock(mtx_);
    *nullable_unique_p_ = i;
  }

  int ReadFromNotNullSharedPointer() {
    std::lock_guard lock(mtx_);
    return *not_null_shared_p_;
  }

  void WriteToNotNullSharedPointer(const int i) {
    std::lock_guard lock(mtx_);
    *not_null_shared_p_ = i;
  }

  int ReadFromNullableSharedPointer() {
    std::lock_guard lock(mtx_);
    return *nullable_shared_p_;
  }

  void WriteToNullableSharedPointer(const int i) {
    std::lock_guard lock(mtx_);
    *nullable_shared_p_ = i;
  }

 private:
  std::mutex mtx_;

  int i_ RST_GUARDED_BY(mtx_) = 0;

  int* p_ RST_PT_GUARDED_BY(mtx_) = nullptr;

  std::unique_ptr<int> unique_p_ RST_PT_GUARDED_BY(mtx_);

  std::shared_ptr<int> shared_p_ RST_PT_GUARDED_BY(mtx_);

  NotNull<int*> not_null_p_ RST_PT_GUARDED_BY(mtx_) = &i_;
  Nullable<int*> nullable_p_ RST_PT_GUARDED_BY(mtx_);

  NotNull<std::unique_ptr<int>> not_null_unique_p_ RST_PT_GUARDED_BY(mtx_) =
      std::make_unique<int>();
  Nullable<std::unique_ptr<int>> nullable_unique_p_ RST_PT_GUARDED_BY(mtx_);

  NotNull<std::shared_ptr<int>> not_null_shared_p_ RST_PT_GUARDED_BY(mtx_) =
      std::make_shared<int>();
  Nullable<std::shared_ptr<int>> nullable_shared_p_ RST_PT_GUARDED_BY(mtx_);
};

}  // namespace
}  // namespace rst
