//
// Created by jinhai on 23-5-27.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {
class Database {
public:
    static inline SharedPtr<Database>
    Open(const String& dir_name);
public:
    Database() = default;
    ~Database();

private:
    i32 locker_fd_{};
};

}


