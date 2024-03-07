////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2018 ArangoDB GmbH, Cologne, Germany
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
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <shared.hpp>

namespace irs {

template <class T>
struct CRCBase {
    T tab[256];
    explicit CRCBase(T polynomial) {
        for (size_t i = 0; i < 256; ++i) {
            T c = static_cast<T>(i);
            for (size_t j = 0; j < 8; ++j)
                c = c & 1 ? polynomial ^ (c >> 1) : c >> 1;
            tab[i] = c;
        }
    }
};

constexpr uint32_t CRC32_IEEE = 0xEDB88320;

class crc32c {
  // TODO(MBkkt) kCrc32Xor == 0 is incorrect, we ignore all first zero bytes,
  //  We can fix it with new directory format and kCrc32Xor = 0xffffffff
  static constexpr uint32_t kInitialRemainder = 0xFFFFFFFF;
  static constexpr uint32_t kFinalXorValue = 0xFFFFFFFF;

 public:
  explicit crc32c(uint32_t seed = 0) noexcept : value_{kInitialRemainder} {}

  IRS_FORCE_INLINE void process_bytes(const void* data, size_t size) noexcept {
    static CRCBase<uint32_t> base{CRC32_IEEE};
    for (size_t i = 0; i < size; ++i)
      value_ = base.tab[(value_ ^ static_cast<const unsigned char*>(data)[i]) & 0xff] ^ (value_ >> 8);
  }

  IRS_FORCE_INLINE void process_block(const void* begin,
                                      const void* end) noexcept {
    process_bytes(begin, std::distance(static_cast<const char*>(begin),
                                       static_cast<const char*>(end)));
  }

  IRS_FORCE_INLINE uint32_t checksum() const noexcept {
    return value_ ^ kFinalXorValue;
  }

  uint32_t value_;
};

}  // namespace irs
