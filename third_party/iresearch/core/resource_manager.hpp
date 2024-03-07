////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2023 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Andrei Lobov
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include "shared.hpp"
#include "utils/managed_allocator.hpp"

namespace irs {

struct IResourceManager {
  static IResourceManager kNoop;
#ifdef IRESEARCH_DEBUG
  static IResourceManager kForbidden;
#endif

  IResourceManager() = default;
  virtual ~IResourceManager() = default;

  IResourceManager(const IResourceManager&) = delete;
  IResourceManager operator=(const IResourceManager&) = delete;

  virtual void Increase([[maybe_unused]] size_t v) {
    IRS_ASSERT(this != &kForbidden);
    IRS_ASSERT(v != 0);
  }

  virtual void Decrease([[maybe_unused]] size_t v) noexcept {
    IRS_ASSERT(this != &kForbidden);
    IRS_ASSERT(v != 0);
  }

  IRS_FORCE_INLINE void DecreaseChecked(size_t v) noexcept {
    if (v != 0) {
      Decrease(v);
    }
  }
};

struct ResourceManagementOptions {
  static ResourceManagementOptions kDefault;

  IResourceManager* transactions{&IResourceManager::kNoop};
  IResourceManager* readers{&IResourceManager::kNoop};
  IResourceManager* consolidations{&IResourceManager::kNoop};
  IResourceManager* file_descriptors{&IResourceManager::kNoop};
  IResourceManager* cached_columns{&IResourceManager::kNoop};
};

template<typename T>
struct ManagedTypedAllocator
  : ManagedAllocator<std::allocator<T>, IResourceManager> {
  using Base = ManagedAllocator<std::allocator<T>, IResourceManager>;
  explicit ManagedTypedAllocator()
    : Base(
#if !defined(_MSC_VER) && defined(IRESEARCH_DEBUG)
        IResourceManager::kForbidden
#else
        IResourceManager::kNoop
#endif
      ) {
  }
  using Base::Base;
};

template<typename T>
using ManagedVector = std::vector<T, ManagedTypedAllocator<T>>;

}  // namespace irs
