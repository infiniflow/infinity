//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "common/default_values.h"
#include "common/types/type_info.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class BlobInfo : public TypeInfo {
public:

    static SharedPtr<BlobInfo>
    Make(i64 limit);

    static SharedPtr<BlobInfo>
    Make();

    explicit
    BlobInfo(SizeT limit) : TypeInfo(TypeInfoType::kBlob), length_limit_(limit) {}

    ~BlobInfo() override = default;

    bool
    operator==(const TypeInfo& other) const override;

    [[nodiscard]] SizeT
    Size() const override {  return length_limit_; }

    [[nodiscard]] SizeT
    length_limit() const { return length_limit_; }

private:
    SizeT length_limit_ {0};
};

}
