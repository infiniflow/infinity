//
// Created by JinHai on 2022/11/8.
//

#include "bitmask_buffer.h"

namespace infinity {

// use to indicate if the null or missing is set
struct Bitmask {
public:
    Bitmask();
    ~Bitmask();

    void
    Reset();

    void
    Initialize(size_t count);

    void
    DeepCopy(const Bitmask& ptr);

    void
    ShallowCopy(const Bitmask& ptr);

    [[nodiscard]] u64*
    GetData() const {
        return data_ptr_;
    }

    void
    Resize(size_t new_capacity);

    String
    ToString(size_t from, size_t to);

    [[nodiscard]] bool
    IsAllTrue() const;

    bool
    IsTrue(size_t row_index);

    void
    SetTrue(size_t row_index);

    void
    SetFalse(size_t row_index);

    void
    Set(size_t row_index, bool valid);

    void
    SetAllTrue();

    [[nodiscard]] size_t
    CountTrue() const;

    [[nodiscard]] size_t
    CountFalse() const { return count_ - CountTrue(); }

    void
    Merge(const Bitmask &other);

    [[nodiscard]] size_t
    count() const { return count_; }

private:
    u64* data_ptr_ {nullptr};
    SharedPtr<BitmaskBuffer> buffer_ptr {nullptr};
    size_t count_ {0};
};

}
