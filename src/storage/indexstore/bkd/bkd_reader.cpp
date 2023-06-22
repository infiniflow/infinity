#include "bkd_reader.h"
#include "bkd_writer.h"
#include "bkd_util.h"
#include "common/utility/infinity_assert.h"

#include <cassert>

namespace infinity{

BKDReader::BKDReader(FileReader *in) {
    in_ = std::unique_ptr<FileReader>(in);
}

int BKDReader::ReadMeta(FileReader* meta_in) {
    type_ = meta_in->ReadInt();
    index_fp_ = meta_in->ReadLong();
    if (0 == index_fp_) {
        return 0;
    } else {
        return 1;
    }
}

void BKDReader::ReadIndex(FileReader* index_in) {
    num_data_dims_ = index_in->ReadVInt();
    num_index_dims_ = num_data_dims_;

    max_points_in_leaf_node_ = index_in->ReadVInt();
    bytes_per_dim_ = index_in->ReadVInt();
    bytes_per_index_entry_ = (num_data_dims_ == 1) && version_ >= BKDWriter::VERSION_IMPLICIT_SPLIT_DIM_1D ? bytes_per_dim_ : bytes_per_dim_ + 1;
    packed_bytes_length_ = num_data_dims_ * bytes_per_dim_;
    packed_index_bytes_length_ = num_index_dims_ * bytes_per_dim_;

    num_leaves_ = index_in->ReadVInt();
    assert(num_leaves_ > 0);
    leaf_node_offset_ = num_leaves_;

    min_packed_value_ = std::vector<uint8_t>(packed_index_bytes_length_);
    max_packed_value_ = std::vector<uint8_t>(packed_index_bytes_length_);

    index_in->Read((char*)min_packed_value_.data(), packed_index_bytes_length_);
    index_in->Read((char*)max_packed_value_.data(), packed_index_bytes_length_);

    for (int32_t dim = 0; dim < num_index_dims_; dim++) {
        if (BKDUtil::CompareUnsigned(min_packed_value_.data(), dim * bytes_per_dim_,
                                          dim * bytes_per_dim_ + bytes_per_dim_,
                                          max_packed_value_.data(),
                                          dim * bytes_per_dim_,
                                          dim * bytes_per_dim_ + bytes_per_dim_) > 0) {
        	throw StorageException("min_packed_value > max_packed_value for dim=" + std::to_string(dim));
        }
    }

    point_count_ = index_in->ReadVLong();

    doc_count_ = index_in->ReadVInt();

    int32_t num_bytes = index_in->ReadVInt();
    meta_offset_ = index_in->GetFilePointer();

    packed_index_ = std::make_shared<std::vector<uint8_t>>(num_bytes);
    index_in->Read((char*)packed_index_->data(), num_bytes);
    leaf_block_fps_.clear();
    split_packed_values_.clear();
}

}