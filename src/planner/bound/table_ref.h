//
// Created by JinHai on 2022/9/12.
//

#pragma once

namespace infinity {

enum class TableRefType {
    kInvalid,
    kBaseTable,
    kSubquery,
    kCrossProduct,
    kJoin,
};

class TableRef {
public:
    explicit TableRef(TableRefType type): type_(type) {}

    TableRefType type_{TableRefType::kInvalid};
};

}
