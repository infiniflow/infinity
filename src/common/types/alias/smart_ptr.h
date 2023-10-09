//
// Created by jinhai on 23-9-30.
//

#pragma once

#include <memory>

namespace infinity {

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T, typename... Args>
inline SharedPtr<T> MakeShared(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T, typename... Args>
inline UniquePtr<T> MakeUnique(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

} // namespace infinity
