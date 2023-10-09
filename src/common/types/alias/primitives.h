//
// Created by jinhai on 23-9-30.
//

#pragma once

#include <cstdint>
#include <cstring>

namespace infinity {

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using idx_t = u64;

using f32 = float;
using f64 = double;

using offset_t = int64_t;

using ptr_t = char *;
using const_ptr_t = const char *;
using char_t = char;
using SizeT = size_t;

using TxnTimeStamp = u64;

} // namespace infinity
