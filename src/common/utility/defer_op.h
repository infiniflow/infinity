//
// Created by jinhai on 23-6-17.
//

#pragma once

#include <utility>

namespace infinity {

template<typename FN>
class DeferFn {
public:
    explicit
    DeferFn(FN func) : func_(std::move(func)) {}

    ~DeferFn() noexcept {
        func_();
    }

private:
    FN func_;
};

}