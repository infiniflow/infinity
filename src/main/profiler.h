//
// Created by JinHai on 2022/9/5.
//

#pragma once

#include <chrono>
#include <string>

namespace infinity {

class Profiler {
public:
    Profiler() = default;

    // Start the profiler
    void Begin();

    // End the profiler
    void End();

    [[nodiscard]] std::string ElapsedToString() const;

    // Return the elapsed time from begin, if the profiler is ended, it will return total elapsed time.
    [[nodiscard]] inline int64_t Elapsed() const { return ElapsedInternal().count(); }

private:
    [[nodiscard]] static inline std::chrono::time_point<std::chrono::high_resolution_clock>
    Now() {
        return std::chrono::high_resolution_clock::now();
    }

    [[nodiscard]] std::chrono::nanoseconds
    ElapsedInternal() const;

    std::chrono::time_point<std::chrono::high_resolution_clock> begin_ts_{};
    std::chrono::time_point<std::chrono::high_resolution_clock> end_ts_{};

    bool finished_ = false;
};


}
