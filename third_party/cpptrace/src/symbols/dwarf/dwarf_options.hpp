#ifndef DWARF_OPTIONS_HPP
#define DWARF_OPTIONS_HPP

#include "utils/optional.hpp"

#include <cstddef>

namespace cpptrace {
namespace detail {
    optional<std::size_t> get_dwarf_resolver_line_table_cache_size();
    bool get_dwarf_resolver_disable_aranges();
}
}

#endif
