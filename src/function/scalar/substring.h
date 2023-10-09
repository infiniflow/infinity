//
// Created by jinhai on 23-3-20.
//

#pragma once

#include "function/function_set.h"
#include "function/scalar_function.h"
#include "storage/meta/catalog.h"

namespace infinity {

void RegisterSubstringFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}
