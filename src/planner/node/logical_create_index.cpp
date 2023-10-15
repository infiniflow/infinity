
#include "planner/node/logical_create_index.h"
#include "common/types/data_type.h"
#include "common/types/logical_type.h"
#include "parser/definition/index_def.h"
#include "planner/column_binding.h"

namespace infinity {

Vector<ColumnBinding> LogicalCreateIndex::GetColumnBindings() const { return {}; };

SharedPtr<Vector<String>> LogicalCreateIndex::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    result->emplace_back("OK");
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalCreateIndex::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
    result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    return result_type;
}

String LogicalCreateIndex::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Create Table: " << *schema_name_ << "." << index_definition_->ToString();
    space += arrow_str.size();

    return ss.str();
}
} // namespace infinity