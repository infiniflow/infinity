#pragma once

#include "faiss/Index.h"
#include "storage/buffer/buffer_handle.h"

namespace infinity {

class ObjectHandle {
protected:
    BufferHandle *buffer_handle_{};

public:
    ObjectHandle() {}

    explicit ObjectHandle(BufferHandle *buffer_handle);

    ObjectHandle(const ObjectHandle &other) = delete;

    ObjectHandle &operator=(const ObjectHandle &other) = delete;

    ObjectHandle(ObjectHandle &&other);

    ObjectHandle &operator=(ObjectHandle &&other);

    ~ObjectHandle();

    SharedPtr<String> GetDir() const;
};

class CommonObjectHandle : public ObjectHandle {
    ptr_t ptr_{};

public:
    CommonObjectHandle() {}

    explicit CommonObjectHandle(BufferHandle *buffer_handle);

    CommonObjectHandle(const CommonObjectHandle &other) = delete;

    CommonObjectHandle &operator=(const CommonObjectHandle &other) = delete;

    CommonObjectHandle(CommonObjectHandle &&other);

    CommonObjectHandle &operator=(CommonObjectHandle &&other);

    [[nodiscard]] ptr_t GetData();
};

class IndexObjectHandle : public ObjectHandle {
    faiss::Index *index_{};

public:
    explicit IndexObjectHandle(BufferHandle *buffer_handle);

    IndexObjectHandle(const IndexObjectHandle &other) = delete;

    IndexObjectHandle &operator=(const IndexObjectHandle &other) = delete;

    IndexObjectHandle(IndexObjectHandle &&other);

    IndexObjectHandle &operator=(IndexObjectHandle &&other);

    [[nodiscard]] faiss::Index *GetIndex();
};

} // namespace infinity