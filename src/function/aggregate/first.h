//
// Created by jinhai on 23-4-7.
//

#pragma once

#include "function/aggregate_function.h"
#include "storage/meta/catalog.h"

namespace infinity {

void RegisterFirstFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}