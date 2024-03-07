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

export module varchar_layout;


import stl;
import internal_types;

namespace infinity {
// TODO shenyushi: Is this file in an appropriate location?

#pragma pack(1)

export struct VarcharLayout {
    u64 is_value_:1{};
    u64 length_:23{};

    struct ShortInfo {
        Array<char_t, VARCHAR_INLINE_LEN> data{};
    };

    struct LongInfo {
        Array<char_t, VARCHAR_PREFIX_LEN> prefix_{};

        u16 file_idx_{};

        u16 _gap_{};

        u32 file_offset_{};
    };

    static_assert(sizeof(ShortInfo) == sizeof(LongInfo));

    union {
        ShortInfo short_info_;
        LongInfo long_info_;
    } u;
};

static_assert(sizeof(VarcharLayout) == sizeof(VarcharT));

#pragma pack()

} // namespace infinity