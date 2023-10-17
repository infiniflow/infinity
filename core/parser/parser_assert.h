//
// Created by jinhai on 23-10-17.
//

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

} // namespace infinty
