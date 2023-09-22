#include "common/types/info/varchar_info.h"
#include "common/default_values.h"
#include "common/types/internal_types.h"
#include "common/types/type_info.h"
#include "common/utility/infinity_assert.h"
namespace infinity {

SharedPtr<VarcharInfo>
VarcharInfo::Make(i64 dimension) {
    TypeAssert(dimension <= MAX_VARCHAR_SIZE, "Varchar dimension should less than " + std::to_string(MAX_VARCHAR_SIZE));
    return MakeShared<VarcharInfo>(dimension);
}

bool
VarcharInfo::operator==(const TypeInfo& other) const {
    if (other.type() != TypeInfoType::kVarchar) {
        return false;
    }
    auto varchar_info = static_cast<const VarcharInfo &>(other);
    return varchar_info.dimension_ == this->dimension_;
}

nlohmann::json
VarcharInfo::Serialize() const {
    nlohmann::json res;
    res["dimension"] = dimension_;
    return res;
}

}