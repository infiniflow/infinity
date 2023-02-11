//
// Created by JinHai on 2022/10/25.
//

#pragma once

namespace infinity {

class DummyTableRef: public TableRef  {
public:
    explicit DummyTableRef(u64 table_index) : TableRef(TableRefType::kDummy, table_index, "Dummy") {}
};

}
