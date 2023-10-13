//
// Created by jinhai on 23-3-19.
//

#pragma once

#include "common/types/alias/smart_ptr.h"

namespace infinity {

class NewCatalog;

void RegisterExtractFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}
