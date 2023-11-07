

module;

import stl;
import parser;

export module index_data;

namespace infinity {

export template <typename CentroidsDataType, typename VectorDataType = CentroidsDataType>
struct IVFFlatIndexData {
    u32 dimension_;
    u32 partition_num_;
    u32 data_num_{};
    Vector<CentroidsDataType> centroids_;
    Vector<Vector<VectorDataType>> vectors_;
    Vector<Vector<RowID>> ids_;
    IVFFlatIndexData(u32 dimension, u32 partition_num)
        : dimension_(dimension), partition_num_(partition_num), centroids_(partition_num_ * dimension_), vectors_(partition_num_),
          ids_(partition_num_) {}
};
} // namespace infinity