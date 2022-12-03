//
// Created by JinHai on 2022/11/24.
//

#include "array_mixed_type.h"
#include "mixed_array_value.h"
#include "mixed_type.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

void
ArrayMixedType::Reset(bool in_constructor) {
    if(in_constructor) {
        ptr = nullptr;
        count = 0;
        return ;
    }

    if(ptr != nullptr) {

        auto* array_value_ptr = (MixedArrayValue*)(this->ptr);
        for(u16 i = 0; i < count; ++ i) {
            MixedType& slot_ref = array_value_ptr->array[i];
            slot_ref.Reset();
        }

        delete[] ptr;
        GlobalResourceUsage::DecrRawMemCount();

        ptr = nullptr;
        count = 0;
    }
}

}
