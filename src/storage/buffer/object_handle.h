//
// Created by jinhai on 23-7-21.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/buffer/buffer_handle.h"

namespace infinity {

enum class ObjectType {
    kColumnBlockData,
};

class ObjectHandle {
public:
    explicit
    ObjectHandle() {}

    explicit
    ObjectHandle(BufferHandle *buffer_handle)
            : buffer_handle_(buffer_handle) {}

    ObjectHandle(const ObjectHandle &other) = delete;

    ObjectHandle& operator=(const ObjectHandle &other) = delete;

    ObjectHandle(ObjectHandle &&other);

    ObjectHandle& operator=(ObjectHandle &&other);

    ~ObjectHandle();

    [[nodiscard]] ptr_t
    GetData();

    [[nodiscard]] SharedPtr<String>&
    GetDir() {
        return buffer_handle_->current_dir_;
    }

private:
    BufferHandle *buffer_handle_{};

    ptr_t ptr_{};

};


}
