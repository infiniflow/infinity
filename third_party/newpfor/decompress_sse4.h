/*
 * Copyright 2014-present Alibaba Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <stdint.h>

namespace indexlib {
void decompress_sse4_c0(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c1(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c2(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c3(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c4(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c5(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c6(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c7(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c8(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c9(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c10(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c11(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c12(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c13(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c14(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c15(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c16(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c17(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c18(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c19(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c20(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c21(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c22(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c23(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c24(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c25(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c26(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c27(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c28(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c29(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c30(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c31(uint32_t* dest, const uint32_t* encode, uint32_t n);
void decompress_sse4_c32(uint32_t* dest, const uint32_t* encode, uint32_t n);
}
