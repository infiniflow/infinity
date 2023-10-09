//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function/aggregate_function.h"
#include "storage/meta/catalog.h"

namespace infinity {

void RegisterMinFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}