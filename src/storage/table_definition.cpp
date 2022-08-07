//
// Created by JinHai on 2022/8/8.
//

#include "common/utility/asserter.h"
#include "table_definition.h"

namespace infinity {

size_t
TableDefinition::GetIdByName(const std::string& name) const {
    const auto iter = std::find_if(columns_.begin(), columns_.end(),
                 [&](const ColumnDefinition& column) { return column.name() == name; });
    Assert(iter != columns_.end(), "Can't find column '" + name + "'");
    return std::distance(columns_.begin(), iter);
}

}