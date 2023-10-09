//
// Created by jinhai on 23-3-18.
//

#pragma once

#include "function/function_set.h"
#include "function/scalar_function.h"
#include "storage/meta/catalog.h"

namespace infinity {

void RegisterLikeFunction(const UniquePtr<NewCatalog> &catalog_ptr);

void RegisterNotLikeFunction(const UniquePtr<NewCatalog> &catalog_ptr);

} // namespace infinity
