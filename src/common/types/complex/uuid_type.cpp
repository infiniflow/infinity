//
// Created by JinHai on 2022/10/31.
//

#include "uuid_type.h"

namespace infinity {

UuidType::UuidType(const UuidType& other) {
    memcpy(this->body, other.body, LENGTH);
}

UuidType::UuidType(UuidType&& other) noexcept {
    memcpy(this->body, other.body, LENGTH);
    other.Reset();
}

UuidType&
UuidType::operator=(const UuidType& other) {
    if(this == &other) return *this;
    memcpy(this->body, other.body, LENGTH);
    return *this;
}

UuidType&
UuidType::operator=(UuidType&& other) noexcept {
    if(this == &other) return *this;
    memcpy(this->body, other.body, LENGTH);
    other.Reset();
    return *this;
}

}
