//
// Created by jinhai on 23-7-21.
//

#pragma once

#include "buffer_handle.h"
#include "common/types/internal_types.h"

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

private:
    BufferHandle *buffer_handle_{};

    ptr_t ptr_{};

};

}
