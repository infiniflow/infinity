//
// Created by jinhai on 23-4-7.
//

#pragma once

#include "common/types/alias/smart_ptr.h"

namespace infinity {

class NewCatalog;

void RegisterFirstFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}