//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include <utility>

#include "common/types/type_info.h"
#include "common/types/logical_type.h"
#include "common/types/data_type.h"
#include "common/types/alias/smart_ptr.h"

namespace infinity {

class ArrayInfo : public TypeInfo {
public:

    static SharedPtr<ArrayInfo>
    Make(DataType elem_type) {
        return MakeShared<ArrayInfo>(std::move(elem_type));
    }

    explicit ArrayInfo(DataType elem_type) :
            TypeInfo(TypeInfoType::kArray), elem_type_(std::move(elem_type)) {}

    ~ArrayInfo() override = default;

    bool
    operator==(const TypeInfo& other) const override;

    // Array always costs 8 bytes. Real data is stored in heap memory.
    [[nodiscard]]  size_t
    Size() const override {
        return 8u;
    }

    [[nodiscard]] nlohmann::json
    Serialize() const override;

    [[nodiscard]] inline String
    ToString() const override {
        return "array(" + elem_type_.ToString() + ")";
    }
private:
    DataType elem_type_;
};

}
