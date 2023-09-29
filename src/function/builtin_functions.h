//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function_set.h"
#include "storage/meta/catalog.h"

namespace infinity {

class BuiltinFunctions : public std::enable_shared_from_this<BuiltinFunctions> {
public:
    explicit
    BuiltinFunctions(UniquePtr<NewCatalog>& catalog_ptr);

    void
    Init();


private:
    UniquePtr<NewCatalog>& catalog_ptr_;

private:

    void
    RegisterAggregateFunction();

    void
    RegisterScalarFunction();

    void
    RegisterTableFunction();
};

}
