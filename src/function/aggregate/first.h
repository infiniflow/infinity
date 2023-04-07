//
// Created by jinhai on 23-4-7.
//

#pragma once

#include "function/aggregate_function.h"
#include "storage/catalog.h"

namespace infinity {

void
RegisterFirstFunction(const UniquePtr<Catalog> &catalog_ptr);

}