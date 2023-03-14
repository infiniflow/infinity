//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "catalog.h"
#include <string>


namespace infinity {

class Infinity;

class Storage {
public:
    explicit
    Storage(String data_path);

    [[nodiscard]] inline UniquePtr<Catalog>&
    catalog() noexcept {
        return catalog_;
    }

    void
    Init();

    void
    Uninit();

private:
    String data_path_;
    UniquePtr<Catalog> catalog_;
};

}
