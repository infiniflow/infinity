//
// Created by jinhai on 23-1-12.
//

#pragma once

#include "common/types/alias/smart_ptr.h"

namespace infinity {

class NewCatalog;

void RegisterNotFunction(const UniquePtr<NewCatalog> &catalog_ptr);

}
