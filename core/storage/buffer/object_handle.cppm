
module;

import buffer_handle;
import stl;

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

    SharedPtr<String> GetDir() const;
};

export class CommonObjectHandle : public ObjectHandle {
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
#if 0
export class IndexObjectHandle : public ObjectHandle {
//    faiss::Index *index_{};

public:
    explicit IndexObjectHandle(BufferHandle *buffer_handle);

    IndexObjectHandle(const IndexObjectHandle &other) = delete;

    IndexObjectHandle &operator=(const IndexObjectHandle &other) = delete;

    IndexObjectHandle(IndexObjectHandle &&other);

    IndexObjectHandle &operator=(IndexObjectHandle &&other);

//    [[nodiscard]] faiss::Index *GetIndex();
};
#endif
} // namespace infinity
