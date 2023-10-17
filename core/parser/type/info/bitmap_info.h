//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "type/type_info.h"
#include <memory>

namespace infinity {

class BitmapInfo : public TypeInfo {
public:
    static std::shared_ptr<BitmapInfo> Make(int64_t limit);

    static std::shared_ptr<BitmapInfo> Make();

    explicit BitmapInfo(size_t limit) : TypeInfo(TypeInfoType::kBitmap), length_limit_(limit) {}

    ~BitmapInfo() override = default;

    bool operator==(const TypeInfo &other) const override;

    [[nodiscard]] size_t Size() const override { return length_limit_; }

    [[nodiscard]] size_t length_limit() const { return length_limit_; }

    [[nodiscard]] inline std::string ToString() const override { return "bitmap(" + std::to_string(length_limit_) + ")"; }

private:
    size_t length_limit_{0};
};

} // namespace infinity
