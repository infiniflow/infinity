module;

#include "toml.hpp"

export module infinity_core:tomlplusplus;

namespace infinity {
export namespace toml {
// Toml parser
using ::toml::parse_file;
using ::toml::table;
} // namespace toml
} // namespace infinity