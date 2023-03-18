//
// Created by jinhai on 23-3-18.
//

#pragma once

#include "function/scalar_function.h"
#include "function/function_set.h"
#include "storage/catalog.h"

namespace infinity {

void
RegisterLikeFunction(const UniquePtr<Catalog> &catalog_ptr);

void
RegisterNotLikeFunction(const UniquePtr<Catalog> &catalog_ptr);

}
