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

#pragma once

#include <exception>
#include <string>

namespace infinity {

class ParserException : public std::exception {
public:
    explicit ParserException(std::string message) : message_(std::move(message)) {}
    [[nodiscard]] inline const char *what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

#ifdef INFINITY_DEBUG
#define ParserAssert(is_true, message)                                                                                                               \
    if (!(is_true)) {                                                                                                                                \
        const std::string path = __FILE__;                                                                                                           \
        const auto pos = path.find("/core/");                                                                                                        \
        if (pos != std::string::npos) {                                                                                                              \
            path.substr(pos + 1);                                                                                                                    \
        }                                                                                                                                            \
                                                                                                                                                     \
        std::string errmsg = std::string(message) + " @" + path + ":" + std::to_string(__LINE__);                                                    \
        throw ParserException(errmsg);                                                                                                               \
    }
#else
#define ParserAssert(is_true, message)                                                                                                               \
    if (!(is_true)) {                                                                                                                                \
        std::string errmsg = std::string(message);                                                                                                   \
        throw ParserException(errmsg);                                                                                                               \
    }
#endif

#define ParserError(message) ParserAssert(false, message)

} // namespace infinity
