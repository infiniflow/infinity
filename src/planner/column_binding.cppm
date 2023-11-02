//
// Created by jinhai on 23-2-17.
//

module;

#include <functional>

import stl;

export module column_binding;

namespace infinity {

export struct ColumnBinding {

    ColumnBinding() = default;

    explicit ColumnBinding(SizeT tbl_idx, SizeT col_idx) : table_idx(tbl_idx), column_idx(col_idx) {}

    inline bool operator==(const ColumnBinding &other) const { return table_idx == other.table_idx && column_idx == other.column_idx; }

    SizeT table_idx;
    SizeT column_idx;
};

} // namespace infinity

namespace std {

template <>
class hash<infinity::ColumnBinding> {
public:
    std::size_t operator()(const infinity::ColumnBinding &rhs) const {
        return hash<std::size_t>().operator()(rhs.table_idx) ^ hash<std::size_t>().operator()(rhs.column_idx);
    }
};

} // namespace std
