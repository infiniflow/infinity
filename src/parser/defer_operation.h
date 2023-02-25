//
// Created by jinhai on 23-2-25.
//

#pragma once

#include <utility>

template <class Func>
class DeferOperation {
public:

    inline explicit
    DeferOperation(Func func) : defer_(std::move(func)) {}

    inline
    ~DeferOperation() noexcept {
        defer_();
    }

private:
    Func defer_;
};
