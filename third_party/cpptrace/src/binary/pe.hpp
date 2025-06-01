#ifndef PE_HPP
#define PE_HPP

#include "platform/platform.hpp"
#include "utils/utils.hpp"

#if IS_WINDOWS
#include <cstdint>
#include <string>

namespace cpptrace {
namespace detail {
    Result<std::uintptr_t, internal_error> pe_get_module_image_base(cstring_view object_path);
}
}

#endif

#endif
