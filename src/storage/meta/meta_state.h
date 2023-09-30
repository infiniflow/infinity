//
// Created by jinhai on 23-8-5.
//

#pragma once



namespace infinity {

struct MetaColumnDataState {
    ColumnDataEntry* column_data_{};
};

struct MetaColumnVectorState {
    ColumnVector* column_vector_{};
};

struct MetaDataBlockState {
    DataBlock* data_block_{};
    HashMap<u64, MetaColumnVectorState> column_vector_map_{};
};

struct MetaSegmentState {
    SegmentEntry* segment_entry_{};
    HashMap<u64, MetaColumnDataState> column_data_map_{};
};

struct MetaTableState {
    Vector<MetaDataBlockState> local_blocks_{};
    HashMap<u64, MetaSegmentState> segment_map_{};
};

}
