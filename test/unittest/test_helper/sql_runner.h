//
// Created by jinhai on 23-3-8.
//

#pragma once

#include "parser/sql_parser.h"

#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "executor/physical_planner.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/session.h"
#include "main/stats/global_resource_usage.h"
#include "planner/logical_planner.h"
#include "planner/optimizer.h"

#include "executor/fragment_builder.h"

namespace infinity {

class SQLRunner {

public:
    static SharedPtr<Table> Run(const String &sql_text, bool print = true);
};

} // namespace infinity
