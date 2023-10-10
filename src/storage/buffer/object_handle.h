#pragma once

#include "faiss/Index.h"
#include "common/types/alias/primitives.h"
#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"

namespace infinity {

class BufferHandle;

class ObjectHandle {
protected:
    BufferHandle *buffer_handle_{};

public:
    ObjectHandle() = default;

    explicit ObjectHandle(BufferHandle *buffer_handle);

    ObjectHandle(const ObjectHandle &other) = delete;

    ObjectHandle &operator=(const ObjectHandle &other) = delete;

    ObjectHandle(ObjectHandle &&other) noexcept;

    ObjectHandle &operator=(ObjectHandle &&other) noexcept;

    ~ObjectHandle();

    SharedPtr<String> GetDir() const;
};

class CommonObjectHandle : public ObjectHandle {
    ptr_t ptr_{};

public:
    CommonObjectHandle() {}

    explicit CommonObjectHandle(BufferHandle *buffer_handle);

    ~CommonObjectHandle();

    CommonObjectHandle(const CommonObjectHandle &other) = delete;

    CommonObjectHandle &operator=(const CommonObjectHandle &other) = delete;

    CommonObjectHandle(CommonObjectHandle &&other);

    CommonObjectHandle &operator=(CommonObjectHandle &&other);

    [[nodiscard]] ptr_t GetData();
};

// TODO shenyushi: remove this layer. Use buffer_manager to manage different file instead
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