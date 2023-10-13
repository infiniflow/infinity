//
// Created by JinHai on 2022/7/30.
//

#include "logical_show.h"
#include <sstream>

namespace infinity {

String ToString(ShowType type) {
    switch (type) {
        case ShowType::kShowTables:
            return "Show tables";
        case ShowType::kShowViews:
            return "Show views";
        case ShowType::kShowColumn:
            return "Show column";
        default: {
            PlannerError("Invalid chunk scan type");
        }
    }
}

Vector<ColumnBinding> LogicalShow::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalShow::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalShow::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalShow::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << "-> "
       << "Show: ";
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity