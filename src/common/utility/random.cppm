export module infinity_core:random;

import std.compat;

namespace infinity {

export std::string RandomString(size_t len);

// Return randomized `name`
export std::shared_ptr<std::string> DetermineRandomString(const std::string &parent_dir, const std::string &name);

export std::shared_ptr<std::string> DetermineRandomPath(const std::string &name);

} // namespace infinity