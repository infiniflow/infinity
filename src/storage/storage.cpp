//
// Created by JinHai on 2022/9/14.
//

#include "storage.h"
#include "schema_definition.h"
#include "catalog.h"
#include "function/builtin_functions.h"

namespace infinity {

Storage::Storage(const Config* config_ptr)
    : config_ptr_(config_ptr),
    catalog_(MakeUnique<Catalog>())
    {}

void
Storage::Init() {

    // Update schema need to begin transaction
    SharedPtr<String> schema_name = MakeShared<String>("default");
    SharedPtr<SchemaDefinition> schema_def_ptr = MakeShared<SchemaDefinition>(schema_name, ConflictType::kError);
    catalog_->CreateSchema(schema_def_ptr);
    // Commit transaction

    BuiltinFunctions builtin_functions(catalog_);
    builtin_functions.Init();
}

void
Storage::Uninit() {
    catalog_.reset();
}

}
