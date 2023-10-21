//
// Created by jinhai on 23-3-18.
//

module;

import stl;

export module like;

namespace infinity {

class NewCatalog;

export void RegisterLikeFunction(const UniquePtr<NewCatalog> &catalog_ptr);

export void RegisterNotLikeFunction(const UniquePtr<NewCatalog> &catalog_ptr);

} // namespace infinity
