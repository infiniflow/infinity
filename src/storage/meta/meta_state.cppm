//
// Created by jinhai on 23-8-5.
//

module;

import stl;

export module meta_state;

namespace infinity {

class SegmentEntry;
class BlockEntry;
class BlockColumnEntry;
class DataBlock;
class ColumnVector;

export struct MetaColumnVectorState {
    ColumnVector *column_vector_{};
};

export struct MetaLocalDataState {
    DataBlock *data_block_{};
    HashMap<u64, MetaColumnVectorState> column_vector_map_{};
};

export struct MetaBlockColumnState {
    BlockColumnEntry *block_column_{};
};

export struct MetaBlockState {
    BlockEntry *block_entry_{};
    HashMap<u64, MetaBlockColumnState> column_data_map_{};
};

export struct MetaSegmentState {
    SegmentEntry *segment_entry_{};

    HashMap<i16, MetaBlockState> block_map_{};
};

export struct MetaTableState {
    Vector<MetaLocalDataState> local_blocks_{};

    // segment id->meta segment state
    HashMap<i32, MetaSegmentState> segment_map_{};
};

} // namespace infinity
