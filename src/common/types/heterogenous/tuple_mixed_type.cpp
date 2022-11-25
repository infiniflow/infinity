//
// Created by JinHai on 2022/11/24.
//

#include "tuple_mixed_type.h"
#include "mixed_tuple_value.h"

namespace infinity {

void
TupleMixedType::Reset() {
    if(ptr != nullptr) {
        auto* tuple_value_ptr = (MixedTupleValue*)(this->ptr);
        u32 entry_count = count * 2;
        for(u32 i = 0; i < entry_count; i += 2) {
            MixedType& key_ref = tuple_value_ptr->array[i];
            key_ref.Reset();
            MixedType& value_ref = tuple_value_ptr->array[i + 1];
            value_ref.Reset();
        }

        delete[] ptr;
        ptr = nullptr;
        count = 0;
    }
}

}