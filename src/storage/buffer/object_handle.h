#pragma once

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

public:
    [[nodiscard]] SharedPtr<String> GetDir() const;

    [[nodiscard]] ptr_t GetData();

private:
    ptr_t ptr_{};
};

} // namespace infinity