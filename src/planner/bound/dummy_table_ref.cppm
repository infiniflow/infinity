//
// Created by JinHai on 2022/10/25.
//

module;

import stl;
import parser;
import table_ref;

export module dummy_table_ref;

namespace infinity {

class DummyTableRef : public TableRef {
public:
    explicit DummyTableRef() : TableRef(TableRefType::kDummy, "Dummy") {}
};

} // namespace infinity
