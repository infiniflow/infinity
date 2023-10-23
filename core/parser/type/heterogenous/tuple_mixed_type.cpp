//
// Created by JinHai on 2022/11/24.
//

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