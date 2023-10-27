//
// Created by JinHai on 2022/9/12.
//

module;

import stl;
import parser;
import table_ref;

export module cross_product_table_ref;

namespace infinity {

export class CrossProductTableRef : public TableRef {
public:
    explicit CrossProductTableRef(String alias) : TableRef(TableRefType::kCrossProduct, alias) {}

    //    SharedPtr<BindContext> left_bind_context_{};
    //    SharedPtr<BindContext> right_bind_context_{};

    SharedPtr<TableRef> left_table_ref_{};
    SharedPtr<TableRef> right_table_ref_{};
};

} // namespace infinity