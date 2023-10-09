#pragma once

#include "channel.h"

#include <cstdint>

namespace infinity {
class Worker {
public:
    explicit Worker(std::uint16_t id, std::uint16_t cpu_id);

    ~Worker() noexcept = default;

    void Execute();

    [[nodiscard]] std::uint16_t WorkerID() const noexcept { return id_; }

    [[nodiscard]] std::uint16_t CPUID() const noexcept { return cpu_id_; }

    [[nodiscard]] Channel &GetChannel() noexcept { return channel_; }

    [[nodiscard]] const Channel &GetChannel() const noexcept { return channel_; }

private:
    const std::uint16_t id_;
    const std::uint16_t cpu_id_;

    Channel channel_;

    bool is_running_;
};
} // namespace infinity