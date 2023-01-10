//
// Created by jinhai on 23-1-10.
//

#pragma once

#include "function/scalar_function.h"
#include "function/function_set.h"
#include "storage/catalog.h"

namespace infinity {

void
RegisterPlusFunction(const std::unique_ptr<Catalog> &catalog_ptr);

}

