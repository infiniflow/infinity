//
// Created by jinhai on 23-7-21.
//

#include "object_handle.h"

namespace infinity {

ptr_t
ObjectHandle::GetData() {
    if(ptr_ == nullptr) {
        ptr_ = buffer_handle_->LoadData();
    }
    return ptr_;
}

}