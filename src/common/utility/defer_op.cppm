// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module infinity_core:defer_op;

import :stl;

namespace infinity {

export template <typename FN>
class DeferFn {
public:
    explicit DeferFn(FN func) : func_(std::move(func)) {}

    DeferFn(const DeferFn &) = delete;
    DeferFn(DeferFn &&) : func_(std::exchange(func_, None)) {}

    ~DeferFn() noexcept {
        if (func_) {
            (*func_)();
        }
    }

private:
    Optional<FN> func_;
};

} // namespace infinity