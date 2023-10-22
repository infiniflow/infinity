
module;

import stl;

export module threadutil;

namespace infinity {

// Encapsulates methods for thread access.
class ThreadUtil {
public:
    static bool pin(Thread &thread, const u16 cpu_id);
};

} // namespace infinity