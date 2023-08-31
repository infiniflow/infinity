//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "catalog.h"
#include "main/config.h"
#include <string>


namespace infinity {

class Infinity;

class Storage {
public:
    explicit
    Storage(const Config* config_ptr);

    [[nodiscard]] inline UniquePtr<Catalog>&
    catalog() noexcept {
        return catalog_;
    }

    void
    Init();

    void
    Uninit();

private:
    const Config* config_ptr_{};
    UniquePtr<Catalog> catalog_;
};

}
