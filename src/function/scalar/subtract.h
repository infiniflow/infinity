//
// Created by JinHai on 2022/9/28.
//

#pragma once

#include "function/scalar_function.h"
#include "function/function_set.h"
#include "storage/catalog.h"

namespace infinity {

void
RegisterSubtractFunction(const std::unique_ptr<Catalog> &catalog_ptr);

}
