//
// Created by JinHai on 2022/7/19.
//

#include "infinity.h"

namespace infinity {

Infinity::Infinity()
    : scheduler_(std::make_unique<NaiveScheduler>()),
    catalog_(std::make_unique<Catalog>()),
    config_(std::make_unique<Config>()){

    std::shared_ptr<SchemaDefinition> schema_def_ptr = std::make_shared<SchemaDefinition>("Default", false);
    catalog_->CreateSchema(schema_def_ptr);
}

}
