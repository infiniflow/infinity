module;

export module log;

import stl;

namespace infinity {

class Catalog;

export void RegisterLogFunction(const UniquePtr<Catalog> &catalog_ptr);

}