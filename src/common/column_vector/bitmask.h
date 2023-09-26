//
// Created by JinHai on 2022/11/8.
//

#pragma once

#include "bitmask_buffer.h"
#include "common/types/alias/strings.h"

namespace infinity {

// use to indicate if the null or missing is set
struct Bitmask {
    static inline SharedPtr<Bitmask>
    Make(size_t count) {
        SharedPtr<Bitmask> ptr = MakeShared<Bitmask>();
        ptr->Initialize(count);
        return ptr;
    }

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

    void
    SetAllFalse();

    [[nodiscard]] size_t
    CountTrue() const;

    [[nodiscard]] size_t
    CountFalse() const { return count_ - CountTrue(); }

    void
    Merge(const Bitmask& other);

    [[nodiscard]] size_t
    count() const { return count_; }

    bool operator==(const Bitmask& other) const;
    bool operator!=(const Bitmask& other) const {return !(*this == other);}

    // Estimated serialized size in bytes
    int32_t GetSizeInBytes() const;
    // Write to a char buffer
    void WriteAdv(char *&buf) const;
    // Read from a serialized version
    static SharedPtr<Bitmask> ReadAdv(char *&buf, int32_t maxbytes);

private:
    u64* data_ptr_{nullptr};
    SharedPtr<BitmaskBuffer> buffer_ptr{nullptr};
    size_t count_{0};
};

}
