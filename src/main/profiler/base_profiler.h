//
// Created by JinHai on 2022/9/5.
//

#pragma once

#include "common/types/alias/strings.h"
#include "common/types/alias/primitives.h"

#include <chrono>
#include <string>

namespace infinity {

class BaseProfiler {
public:
    BaseProfiler() = default;

    explicit BaseProfiler(String name) : name_(std::move(name)) {}

    // Start the profiler
    void Begin();

    // End the profiler
    void End();

    [[nodiscard]] String ElapsedToString() const;

    // Return the elapsed time from begin, if the profiler is ended, it will return total elapsed time.
    [[nodiscard]] inline i64 Elapsed() const { return ElapsedInternal().count(); }

    [[nodiscard]] const String &name() const { return name_; }
    void set_name(const String &name) { name_ = name; }

private:
    [[nodiscard]] static inline std::chrono::time_point<std::chrono::high_resolution_clock> Now() {
        return std::chrono::high_resolution_clock::now();
    }

    [[nodiscard]] std::chrono::nanoseconds ElapsedInternal() const;

    std::chrono::time_point<std::chrono::high_resolution_clock> begin_ts_{};
    std::chrono::time_point<std::chrono::high_resolution_clock> max_row_ts_{};

    bool finished_ = false;
    String name_{};
};

} // namespace infinity
