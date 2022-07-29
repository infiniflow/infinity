//
// Created by JinHai on 2022/7/19.
//

#include "infinity.h"

namespace infinity {

Infinity::Infinity()
    : scheduler_(std::make_shared<NaiveScheduler>()), catalog_(std::make_shared<Catalog>()) {

    std::shared_ptr<SchemaDefinition> schema_def_ptr = std::make_shared<SchemaDefinition>("Default", false);
    catalog_->CreateSchema(schema_def_ptr);
}

}
