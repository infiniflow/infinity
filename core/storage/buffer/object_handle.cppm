
module;

import buffer_handle;
import stl;
import third_party;
import faiss_index_ptr;

export module object_handle;

namespace infinity {

export class ObjectHandle {
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

    [[nodiscard]] FaissIndex *GetIndex();

    void WriteFaissIndex(FaissIndexPtr *index);

private:
    [[nodiscard]] void *GetRaw();

    void *ptr_{};
};

} // namespace infinity
