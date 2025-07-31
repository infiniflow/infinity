module;

export module infinity_core:random;

import :stl;

namespace infinity {

export String RandomString(SizeT len);

// Return randomized `name`
export SharedPtr<String> DetermineRandomString(const String &parent_dir, const String &name);

export SharedPtr<String> DetermineRandomPath(const String &name);

} // namespace infinity