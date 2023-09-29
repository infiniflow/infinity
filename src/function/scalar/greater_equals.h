//
// Created by JinHai on 2022/9/28.
//

#pragma once

#include "function/scalar_function.h"
#include "function/function_set.h"
#include "storage/meta/catalog.h"

namespace infinity {

void
RegisterGreaterEqualsFunction(const UniquePtr<NewCatalog>& catalog_ptr);

}
