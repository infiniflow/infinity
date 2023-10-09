//
// Created by jinhai on 23-3-19.
//

#pragma once

#include "function/function_set.h"
#include "function/scalar_function.h"
#include "storage/meta/catalog.h"

namespace infinity {

void RegisterExtractFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}
