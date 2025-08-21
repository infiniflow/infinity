
module;

export module infinity_core:threadutil;

import :infinity_type;

namespace infinity {

// Encapsulates methods for thread access.
export class ThreadUtil {
public:
    static bool pin(std::thread &thread, const u16 cpu_id);
};

} // namespace infinity