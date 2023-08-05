//
// Created by jinhai on 23-8-5.
//

#pragma once

#include "common/types/internal_types.h"
#include "column_data.h"
#include "data_segment.h"

namespace infinity {

struct MetaColumnDataState {
    ColumnData* column_data_{};
};

struct MetaColumnVectorState {
    ColumnVector* column_vector_{};
};

struct MetaDataBlockState {
    DataBlock* data_block_{};
    HashMap<u64, MetaColumnVectorState> column_vector_map_{};
};

struct MetaSegmentState {
    DataSegment* data_segment_{};
    HashMap<u64, MetaColumnDataState> column_data_map_{};
};

struct MetaTableState {
    Vector<MetaDataBlockState> local_blocks_{};
    HashMap<u64, MetaSegmentState> segment_map_{};
};

}
