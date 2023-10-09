//
// Created by JinHai on 2022/10/25.
//

#pragma once

namespace infinity {

class DummyTableRef : public TableRef {
public:
    explicit DummyTableRef() : TableRef(TableRefType::kDummy, "Dummy") {}
};

} // namespace infinity
