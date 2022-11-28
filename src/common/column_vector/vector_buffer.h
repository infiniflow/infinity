//
// Created by JinHai on 2022/11/6.
//
#include "common/types/internal_types.h"
#include "common/types/data_type.h"

namespace infinity {

class VectorBuffer {
public:
    static SharedPtr<VectorBuffer>
    Make(size_t data_type_size, size_t capacity);

public:
    explicit VectorBuffer() {
        GlobalResourceUsage::IncrObjectCount();
    }

    ~VectorBuffer() {
        GlobalResourceUsage::DecrObjectCount();
    }

    void
    Initialize(size_t data_size);

    void
    Copy(ptr_t input, size_t size);

    ptr_t
    GetData() { return data_.get(); }

private:
    bool initialized_ {false};
    UniquePtr<char[]> data_ {nullptr};
    size_t data_size_ {0};
};

}
