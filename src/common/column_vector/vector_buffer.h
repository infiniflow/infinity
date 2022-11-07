//
// Created by JinHai on 2022/11/6.
//
#include "common/types/internal_types.h"
#include "common/types/data_type.h"

namespace infinity {

class VectorBuffer {
public:
    static SharedPtr<VectorBuffer>
    MakeVectorBuffer(DataType data_type, u64 capacity);

public:
    explicit VectorBuffer() = default;

    void
    Initialize(u64 data_size);

    ptr_t
    GetData() { return data_.get(); }

private:
    bool initialized_ {false};
    UniquePtr<i8[]> data_ {nullptr};
};

}
