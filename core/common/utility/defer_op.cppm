//
// Created by jinhai on 23-6-17.
//

module;

import stl;

export module defer_op;

namespace infinity {

export template <typename FN>
class DeferFn {
public:
    explicit DeferFn(FN func) : func_(Move(func)) {}

    ~DeferFn() noexcept { func_(); }

private:
    FN func_;
};

} // namespace infinity