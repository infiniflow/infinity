//
// Created by JinHai on 2022/9/13.
//

#pragma once

#include "parser/table_reference/join_reference.h"
#include "parser/statement/select_statement.h"

namespace infinity {

String
ToString(JoinType type);

String
ToString(OrderType type);

}
