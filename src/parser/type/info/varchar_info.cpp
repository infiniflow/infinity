
#include "type/info/varchar_info.h"
#include "default_value.h"
#include "parser_assert.h"
#include "type/type_info.h"

namespace infinity {

std::shared_ptr<VarcharInfo> VarcharInfo::Make(int64_t dimension) {
    ParserAssert(dimension <= MAX_VARCHAR_SIZE_INTERNAL, "Varchar dimension should less than " + std::to_string(MAX_VARCHAR_SIZE_INTERNAL));
    return std::make_shared<VarcharInfo>(dimension);
}

bool VarcharInfo::operator==(const TypeInfo &other) const {
    if (other.type() != TypeInfoType::kVarchar) {
        return false;
    }
    auto varchar_info = static_cast<const VarcharInfo &>(other);
    return varchar_info.dimension_ == this->dimension_;
}

nlohmann::json VarcharInfo::Serialize() const {
    nlohmann::json res;
    res["dimension"] = dimension_;
    return res;
}

} // namespace infinity
