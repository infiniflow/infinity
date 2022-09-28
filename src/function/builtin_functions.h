//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function_set.h"
#include "storage/catalog.h"

namespace infinity {

class BuiltinFunctions : public std::enable_shared_from_this<BuiltinFunctions> {
public:
    explicit BuiltinFunctions(std::unique_ptr<Catalog>& catalog_ptr);

    void Init();


private:
    std::unique_ptr<Catalog>& catalog_ptr_;

private:

    void RegisterAggregateFunction();
    void RegisterScalarFunction();
    void RegisterTableFunction();
};

}
