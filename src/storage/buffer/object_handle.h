#pragma once

#include "common/types/alias/primitives.h"
#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"

namespace faiss {
class Index;
}

namespace infinity {

class BufferHandle;
class FaissIndexPtr;

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

    [[nodiscard]] inline ptr_t GetData() { return static_cast<ptr_t>(GetRaw()); }

    [[nodiscard]] inline faiss::Index *GetIndex();

    void WriteFaissIndex(FaissIndexPtr *index);

private:
    [[nodiscard]] void *GetRaw();

    void *ptr_{};
};

} // namespace infinity