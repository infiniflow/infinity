//
// Created by jinhai on 23-9-17.
//

module;

import stl;
import table_collection_type;

export module table_collection_detail;

namespace infinity {

export struct TableCollectionDetail {
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_collection_name_{};
    TableCollectionType table_collection_type_;
    i64 column_count_{};
    i64 row_count_{};
    i64 segment_count_{};
    i64 block_count_{};
    i64 segment_capacity_{};
};

} // namespace infinity
