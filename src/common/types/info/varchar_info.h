#pragma once

#include "common/types/type_info.h"


namespace infinity {

class VarcharInfo : public TypeInfo {
public:
    static SharedPtr<VarcharInfo>
    Make(i64 dimension);

    explicit VarcharInfo(i64 dimension) :
            TypeInfo(TypeInfoType::kVarchar), dimension_(dimension) {
        // GlobalResourceUsage::IncrObjectCount();
    }

    ~VarcharInfo() override {
        // GlobalResourceUsage::DecrObjectCount();
    }

    bool
    operator==(const TypeInfo& other) const override;

    [[nodiscard]] i64
    dimension() const { return dimension_; }

    [[nodiscard]] size_t
    Size() const override { return 16u; }

    [[nodiscard]] nlohmann::json
    Serialize() const override;

    [[nodiscard]] inline
    String
    ToString() const override {
        return "varchar(" + std::to_string(dimension_) + ")";
    }

private:
    i64 dimension_;
};
}