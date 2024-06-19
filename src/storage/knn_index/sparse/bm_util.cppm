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

export module bm_util;

import stl;
import infinity_exception;

namespace infinity {

export using BMBlockID = i32;
export using BMBlockOffset = u8;

export using BMDocID = i32;

export enum class BMCompressType : i8 {
    kRaw = 0,
    kCompressed = 1,
    kInvalid = 2,
};

export BMCompressType BMCompressTypeFromString(const String &compress_type_str) {
    if (IsEqual(compress_type_str, "raw")) {
        return BMCompressType::kRaw;
    } else if (IsEqual(compress_type_str, "compressed")) {
        return BMCompressType::kCompressed;
    } else {
        UnrecoverableError("Invalid compress type");
        return BMCompressType::kInvalid;
    }
}

} // namespace infinity