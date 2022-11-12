//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include <utility>

#include "common/types/type_info.h"
#include "common/types/logical_type.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"

namespace infinity {

class ArrayInfo : public TypeInfo {
public:

    static UniquePtr<ArrayInfo>
    Make(DataType elem_type) {
        return MakeUnique<ArrayInfo>(std::move(elem_type));
    }

    explicit ArrayInfo(DataType elem_type) :
            TypeInfo(TypeInfoType::kArray), elem_type_(std::move(elem_type)) {}

    ~ArrayInfo() override = default;

    // Array always costs 8 bytes. Real data is stored in heap memory.
    [[nodiscard]]  size_t
    Size() const override {
        return 8u;
    }

private:
    DataType elem_type_;
};

}
