
#include "type/info/varchar_info.h"
#include "type/type_info.h"
#include "default_value.h"
#include "parser_assert.h"

namespace infinity {

std::shared_ptr<VarcharInfo> VarcharInfo::Make(int64_t dimension) {
    ParserAssert(dimension <= MAX_VARCHAR_SIZE, "Varchar dimension should less than " + std::to_string(MAX_VARCHAR_SIZE));
    return std::make_shared<VarcharInfo>(dimension);
}

bool VarcharInfo::operator==(const TypeInfo &other) const {
    if (other.type() != TypeInfoType::kVarchar) {
        return false;
    }
    auto varchar_info = static_cast<const VarcharInfo &>(other);
    return varchar_info.dimension_ == this->dimension_;
}

} // namespace infinity
