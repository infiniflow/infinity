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

#include "tuple_mixed_type.h"
#include "mixed_tuple_value.h"

namespace infinity {

void TupleMixedType::Reset(bool in_constructor) {
    if (in_constructor) {
        ptr = nullptr;
        count = 0;
        return;
    }

    if (ptr != nullptr) {
        auto *tuple_value_ptr = (MixedTupleValue *)(this->ptr);
        uint32_t entry_count = count * 2;
        for (uint32_t i = 0; i < entry_count; i += 2) {
            MixedType &key_ref = tuple_value_ptr->array[i];
            key_ref.Reset();
            MixedType &value_ref = tuple_value_ptr->array[i + 1];
            value_ref.Reset();
        }

        delete[] ptr;

        ptr = nullptr;
        count = 0;
    }
}

} // namespace infinity