//
// Created by JinHai on 2022/7/28.
//

#include "physical_drop_table.h"

#include "main/infinity.h"

namespace infinity {

void
PhysicalDropTable::Init() {

}

void
PhysicalDropTable::Execute(std::shared_ptr<QueryContext>& query_context) {
    Infinity::instance().catalog()->DeleteTable(*schema_name_, *table_name_);
}


}