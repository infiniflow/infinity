#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <cpptrace/forward.hpp>

#include <vector>
#include <cstdint>

namespace cpptrace {

namespace detail {
    object_frame get_frame_object_info(frame_ptr address);

    std::vector<object_frame> get_frames_object_info(const std::vector<frame_ptr>& addresses);

    object_frame resolve_safe_object_frame(const safe_object_frame& frame);
}
}

#endif
