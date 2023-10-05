//
// Created by jinhai on 23-8-5.
//

#pragma once


namespace infinity {

struct MetaColumnVectorState {
    ColumnVector* column_vector_{};
};

struct MetaLocalDataState {
    DataBlock* data_block_{};
    HashMap<u64, MetaColumnVectorState> column_vector_map_{};
};

struct MetaBlockColumnState {
    BlockColumnEntry* block_column_{};
};


struct MetaBlockState {
    BlockEntry* block_entry_{};
    HashMap<u64, MetaBlockColumnState> column_data_map_{};
};

struct MetaSegmentState {
    SegmentEntry* segment_entry_{};

    HashMap<i16, MetaBlockState> block_map_{};
};

struct MetaTableState {
    Vector<MetaLocalDataState> local_blocks_{};

    // segment id->meta segment state
    HashMap<i32, MetaSegmentState> segment_map_{};
};

}
