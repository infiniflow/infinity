module;

import stl;
import third_party;

export module buffer_handle;

namespace infinity {

class BufferObj;

export class BufferHandle {

public:
    BufferHandle() = default;

    explicit BufferHandle(BufferObj *buffer_handle, const void *data);

    BufferHandle(const BufferHandle &other);

    BufferHandle &operator=(const BufferHandle &other);

    BufferHandle(BufferHandle &&other);

    BufferHandle &operator=(BufferHandle &&other);

    ~BufferHandle();

public:
    [[nodiscard]] const void *GetRaw() { return data_; }

private:
    BufferObj *buffer_obj_{};

    const void *data_{};
};

export class BufferHandleMut {
public:
    BufferHandleMut() = default;

    explicit BufferHandleMut(BufferObj *buffer_handle, void *data);

    BufferHandleMut(const BufferHandleMut &other) = delete;

    BufferHandleMut &operator=(const BufferHandleMut &other) = delete;

    BufferHandleMut(BufferHandleMut &&other);

    BufferHandleMut &operator=(BufferHandleMut &&other);

    ~BufferHandleMut();

public:
    [[nodiscard]] void *GetRaw() { return data_; }

private:
    BufferObj *buffer_obj_{};

    void *data_{};
};

} // namespace infinity