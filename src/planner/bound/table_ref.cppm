//
// Created by JinHai on 2022/9/12.
//

module;

import stl;
import parser;

export module table_ref;

namespace infinity {

export class TableRef {
public:
    explicit TableRef(TableRefType type, String alias) : type_(type), alias_(Move(alias)) {}
    virtual ~TableRef() = default;

    [[nodiscard]] inline TableRefType type() const { return type_; }
    [[nodiscard]] inline const String &alias() const { return alias_; }

    TableRefType type_{TableRefType::kTable};
    String alias_{};
};

} // namespace infinity
