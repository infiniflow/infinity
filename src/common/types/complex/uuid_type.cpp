//
// Created by JinHai on 2022/10/31.
//

#include "uuid_type.h"

namespace infinity {

UuidType::UuidType(const UuidType& other) {
    memcpy(this->body, other.body, UuidType::LENGTH);
}

UuidType::UuidType(UuidType&& other) noexcept {
    memcpy(this->body, other.body, UuidType::LENGTH);
    other.Reset();
}

UuidType&
UuidType::operator=(const UuidType& other) {
    if(this == &other)
        return *this;
    memcpy(this->body, other.body, UuidType::LENGTH);
    return *this;
}

UuidType&
UuidType::operator=(UuidType&& other) noexcept {
    if(this == &other)
        return *this;
    memcpy(this->body, other.body, UuidType::LENGTH);
    other.Reset();
    return *this;
}

bool
UuidType::operator==(const UuidType& other) const {
    if(this == &other)
        return true;
    // TODO: Need to do benchmark of memcmp and strncmp to decide which one should be used here.
    return (memcmp(this->body, other.body, UuidType::LENGTH) == 0);
}

}
