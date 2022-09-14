//
// Created by JinHai on 2022/9/14.
//

#include "storage.h"
#include "schema_definition.h"
#include "catalog.h"
#include "function/builtin_functions.h"

namespace infinity {

Storage::Storage(std::string data_path)
    : data_path_(std::move(data_path)),
    catalog_(std::make_unique<Catalog>())
    {}

void
Storage::Init() {

    // Update schema need to begin transaction
    std::shared_ptr<SchemaDefinition> schema_def_ptr = std::make_shared<SchemaDefinition>("Default", false);
    catalog_->CreateSchema(schema_def_ptr);
    // Commit transaction

    BuiltinFunctions builtin_functions(catalog_);
    builtin_functions.Init();
}

}
