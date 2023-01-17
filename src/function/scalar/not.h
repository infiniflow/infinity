//
// Created by jinhai on 23-1-12.
//

#pragma once

#include "function/scalar_function.h"
#include "function/function_set.h"
#include "storage/catalog.h"

namespace infinity {

void
RegisterNotFunction(const UniquePtr<Catalog> &catalog_ptr);

}

