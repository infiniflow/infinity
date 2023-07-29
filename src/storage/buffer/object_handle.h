//
// Created by jinhai on 23-7-21.
//

#pragma once

#include "buffer_handle.h"

namespace infinity {

enum class ObjectType {
    kColumnBlockData,
};

class ObjectHandle {
public:
    explicit
    ObjectHandle(BufferHandle *buffer_handle)
            : buffer_handle_(buffer_handle) {}

    ~ObjectHandle() {
        buffer_handle_->UnloadData();
    }

    [[nodiscard]] ptr_t
    GetData();

private:
    BufferHandle *buffer_handle_{};
    ptr_t ptr_{};
};


}
