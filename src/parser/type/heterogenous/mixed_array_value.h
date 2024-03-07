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

#pragma once

#include "base_mixed_type.h"
#include "mixed_type.h"

namespace infinity {

struct MixedArrayValue {
public:
    MixedType array[0];
    // private:
    //     static char*
    //     Make(int32_t count);
    //
    //     static void
    //     Set(int32_t index, const MixedType& mixed_value);
    //
    //     static MixedType&
    //     Get(int32_t index);
};

} // namespace infinity