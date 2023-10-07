#include "index_def.h"
#include <sstream>

namespace infinity {
String
IndexDef::ToString() const {
    std::stringstream ss;
    ss << *index_name_ << '-';
    ss << IndexMethodToString(method_type_) << '-';
    ss << *table_name_ << '[';
    size_t column_n = column_names_->size();
    for (size_t i = 0; const auto &column_name : *column_names_) {
        ss << column_name;
        ++i;
        if (i != column_n) {
            ss << ", ";
        }
    }
    ss << ']';

    return ss.str();
}
}// namespace infinity