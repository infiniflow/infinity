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

#include "simd_common_intrin_include.h"

module simd_common_tools;

import stl;
import infinity_exception;

namespace infinity {

struct U8MaskArray {
    alignas(64) u32 masks[256][8] = {};
    U8MaskArray() {
        for (u32 i = 0; i < 256; ++i) {
            for (u32 j = 0; j < 8; ++j) {
                masks[i][j] = (i & (1u << j)) ? std::numeric_limits<u32>::max() : 0;
            }
        }
    }
};

U8MaskPtr GetU8MasksForAVX2() {
    static U8MaskArray u8_mask_array;
    return u8_mask_array.masks;
}


} // namespace infinity
