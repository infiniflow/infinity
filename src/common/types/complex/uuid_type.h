//
// Created by JinHai on 2022/10/31.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

struct UuidType {
public:
    static constexpr i64 LENGTH = 16;

public:
    inline
    UuidType() = default;

    explicit
    UuidType(ptr_t input) {
        Set(input);
    }

    inline
    ~UuidType() {
        Reset();
    }

    UuidType(const UuidType& other);
    UuidType(UuidType&& other) noexcept;
    UuidType& operator=(const UuidType& other);
    UuidType& operator=(UuidType&& other) noexcept;

    bool
    operator==(const UuidType& other) const;

    inline bool
    operator!=(const UuidType& other) const {
        return !operator==(other);
    }

    inline void
    Set(ptr_t input) {
        memcpy(body, input, UuidType::LENGTH);
    }

    [[nodiscard]] inline String
    ToString() const {
        return String(body, UuidType::LENGTH);
    }

    inline void
    Reset() {
        memset(body, 0, UuidType::LENGTH);
    }

public:
    char_t body[UuidType::LENGTH];
};

}
