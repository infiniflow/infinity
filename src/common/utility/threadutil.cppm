
module;

export module infinity_core:threadutil;

import :stl;

namespace infinity {

// Encapsulates methods for thread access.
export class ThreadUtil {
public:
    static bool pin(Thread &thread, const u16 cpu_id);
};

} // namespace infinity