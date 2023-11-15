// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "butil/threading/non_thread_safe_impl.h"

#include "butil/logging.h"

namespace butil {

bool NonThreadSafeImpl::CalledOnValidThread() const {
  return thread_checker_.CalledOnValidThread();
}

NonThreadSafeImpl::~NonThreadSafeImpl() {
  DCHECK(CalledOnValidThread());
}

void NonThreadSafeImpl::DetachFromThread() {
  thread_checker_.DetachFromThread();
}

}  // namespace butil
