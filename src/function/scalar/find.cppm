module;

export module find;

import stl;

namespace infinity {

class Catalog;

export void RegisterFindFunction(const UniquePtr<Catalog> &catalog_ptr);

} // namespace infinity