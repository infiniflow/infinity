//
// Created by JinHai on 2022/9/16.
//

#pragma once

#include "function/function_set.h"
#include "function/scalar_function.h"
#include "storage/meta/catalog.h"

namespace infinity {

void RegisterPowFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}