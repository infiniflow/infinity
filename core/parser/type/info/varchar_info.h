#pragma once

#include "type/type_info.h"
#include <memory>

namespace infinity {

class VarcharInfo : public TypeInfo {
public:
    static std::shared_ptr<VarcharInfo> Make(int64_t dimension);

    explicit VarcharInfo(int64_t dimension) : TypeInfo(TypeInfoType::kVarchar), dimension_(dimension) {
        // GlobalResourceUsage::IncrObjectCount();
    }

    ~VarcharInfo() override {
        // GlobalResourceUsage::DecrObjectCount();
    }

    bool operator==(const TypeInfo &other) const override;

    [[nodiscard]] int64_t dimension() const { return dimension_; }

    [[nodiscard]] size_t Size() const override { return 16u; }

    [[nodiscard]] nlohmann::json Serialize() const override;

    [[nodiscard]] inline std::string ToString() const override { return "varchar(" + std::to_string(dimension_) + ")"; }

private:
    int64_t dimension_;
};
} // namespace infinity