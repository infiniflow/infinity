//
// Created by jinhai on 23-1-5.
//

module;

import singleton;
import stl;
import parser;

export module cast_table;

namespace infinity {

class CastTable : public Singleton<CastTable> {
public:
    CastTable();

public:
    [[nodiscard]] i64 GetCastCost(LogicalType from, LogicalType to) const;

private:
    Array<Array<i64, LogicalType::kInvalid>, LogicalType::kInvalid> matrix_{-1};
};

} // namespace infinity
