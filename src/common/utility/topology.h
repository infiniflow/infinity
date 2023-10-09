#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <numeric>
#include <sched.h>
#include <thread>

namespace infinity {

// Encapsulates methods for retrieving information about the hardware landscape.
// We do not consider NUMAAware right now
class Topology {
public:
    static const std::uint16_t MaxCores = 128U;
    Topology() {}

    virtual ~Topology() = default;

    void build(std::uint16_t max_cores) {
        std::uint16_t cores = std::min(max_cores, Topology::count_cores());
        for (auto i = 0U; i < cores; ++i)
            emplace_back(i);
    }

    void emplace_back(const std::uint16_t core_id) noexcept { core_ids_[size_++] = core_id; }

    std::uint16_t operator[](const std::uint16_t index) const noexcept { return core_ids_[index]; }

    [[nodiscard]] std::uint16_t size() const noexcept { return size_; }

    [[nodiscard]] std::uint16_t max_cpu_id() const noexcept { return *std::max_element(core_ids_.cbegin(), core_ids_.cbegin() + size_); }

    // return Core where the caller is running.
    static std::uint16_t cpu_id() {
#if defined(__APPLE__)
        return 0;
#else
        return std::uint16_t(sched_getcpu());
#endif
    }

    // return Number of available cores.
    static std::uint16_t count_cores() { return std::uint16_t(std::thread::hardware_concurrency()); }

private:
    std::uint16_t size_{0U};

    std::array<std::uint16_t, MaxCores> core_ids_;
};
} // namespace infinity