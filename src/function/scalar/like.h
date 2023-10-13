//
// Created by jinhai on 23-3-18.
//

#pragma once

#include "common/types/alias/smart_ptr.h"

namespace infinity {

class NewCatalog;

void RegisterLikeFunction(const UniquePtr<NewCatalog> &catalog_ptr);

void RegisterNotLikeFunction(const UniquePtr<NewCatalog> &catalog_ptr);

} // namespace infinity
