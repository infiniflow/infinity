module;

export module truncate;

import stl;

namespace infinity {

class Catalog;

export void RegisterTruncateFunction(const UniquePtr<Catalog> &catalog_ptr);

} // namespace infinity
