//
// Created by jinhai on 23-3-19.
//

#pragma once

#include "function/scalar_function.h"
#include "function/function_set.h"
#include "storage/catalog.h"

namespace infinity {

void
RegisterExtractFunction(const UniquePtr<Catalog> &catalog_ptr);

}

