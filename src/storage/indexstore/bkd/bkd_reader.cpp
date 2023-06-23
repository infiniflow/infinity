#include "bkd_reader.h"
#include "bkd_writer.h"
#include "bkd_util.h"
#include "legacy_index_tree.h"
#include "packed_index_tree.h"
#include "common/utility/infinity_assert.h"

#include <cassert>

namespace infinity{

BKDVisitor::BKDVisitor(
    roaring::Roaring* hits, 
    BKDQueryType query_type)
    : hits_(hits), num_hits_(0), query_type_(query_type){
}

void BKDVisitor::SetReader(BKDReader* reader) {
    reader_ = reader;
}

Relation BKDVisitor::Compare(
    std::vector<uint8_t>& min_packed,
    std::vector<uint8_t>& max_packed) {
    bool crosses = false;

    for (int dim = 0; dim < reader_->num_data_dims_; dim++) {
        int offset = dim * reader_->bytes_per_dim_;

        if (query_type_ == BKDQueryType::LESS_THAN) {
            if (BKDUtil::CompareUnsigned(
                        min_packed.data(), offset, offset + reader_->bytes_per_dim_,
                        (const uint8_t*)query_max_.c_str(), offset,
                        offset + reader_->bytes_per_dim_) >= 0) {
                return Relation::CELL_OUTSIDE_QUERY;
            }
        } else if (query_type_ == BKDQueryType::GREATER_THAN) {
            if (BKDUtil::CompareUnsigned(
                        max_packed.data(), offset, offset + reader_->bytes_per_dim_,
                        (const uint8_t*)query_min_.c_str(), offset,
                        offset + reader_->bytes_per_dim_) <= 0) {
                return Relation::CELL_OUTSIDE_QUERY;
            }
        } else {
            if (BKDUtil::CompareUnsigned(
                        min_packed.data(), offset, offset + reader_->bytes_per_dim_,
                        (const uint8_t*)query_max_.c_str(), offset,
                        offset + reader_->bytes_per_dim_) > 0 ||
                BKDUtil::CompareUnsigned(
                        max_packed.data(), offset, offset + reader_->bytes_per_dim_,
                        (const uint8_t*)query_min_.c_str(), offset,
                        offset + reader_->bytes_per_dim_) < 0) {
                return Relation::CELL_OUTSIDE_QUERY;
            }
        }
        if (query_type_ == BKDQueryType::LESS_THAN ||
            query_type_ == BKDQueryType::GREATER_THAN) {
            crosses |= BKDUtil::CompareUnsigned(
                               min_packed.data(), offset, offset + reader_->bytes_per_dim_,
                               (const uint8_t*)query_min_.c_str(), offset,
                               offset + reader_->bytes_per_dim_) <= 0 ||
                       BKDUtil::CompareUnsigned(
                               max_packed.data(), offset, offset + reader_->bytes_per_dim_,
                               (const uint8_t*)query_max_.c_str(), offset,
                               offset + reader_->bytes_per_dim_) >= 0;
        } else {
            crosses |= BKDUtil::CompareUnsigned(
                               min_packed.data(), offset, offset + reader_->bytes_per_dim_,
                               (const uint8_t*)query_min_.c_str(), offset,
                               offset + reader_->bytes_per_dim_) < 0 ||
                       BKDUtil::CompareUnsigned(
                               max_packed.data(), offset, offset + reader_->bytes_per_dim_,
                               (const uint8_t*)query_max_.c_str(), offset,
                               offset + reader_->bytes_per_dim_) > 0;
        }
    }
    if (crosses) {
        return Relation::CELL_CROSSES_QUERY;
    } else {
        return Relation::CELL_INSIDE_QUERY;
    }
}

bool BKDVisitor::Matches(uint8_t* packed_value) {
    for (int dim = 0; dim < reader_->num_data_dims_; dim++) {
        int offset = dim * reader_->bytes_per_dim_;
        if (query_type_ == BKDQueryType::LESS_THAN) {
            if (BKDUtil::CompareUnsigned(
                        packed_value, offset, offset + reader_->bytes_per_dim_,
                        (const uint8_t*)query_max_.c_str(), offset,
                        offset + reader_->bytes_per_dim_) >= 0) {
                // Doc's value is too high, in this dimension
                return false;
            }
        } else if (query_type_ == BKDQueryType::GREATER_THAN) {
            if (BKDUtil::CompareUnsigned(
                        packed_value, offset, offset + reader_->bytes_per_dim_,
                        (const uint8_t*)query_min_.c_str(), offset,
                        offset + reader_->bytes_per_dim_) <= 0) {
                // Doc's value is too high, in this dimension
                return false;
            }
        } else {
            if (BKDUtil::CompareUnsigned(
                        packed_value, offset, offset + reader_->bytes_per_dim_,
                        (const uint8_t*)query_min_.c_str(), offset,
                        offset + reader_->bytes_per_dim_) < 0) {
                // Doc's value is too low, in this dimension
                return false;
            }
            if (BKDUtil::CompareUnsigned(
                        packed_value, offset, offset + reader_->bytes_per_dim_,
                        (const uint8_t*)query_max_.c_str(), offset,
                        offset + reader_->bytes_per_dim_) > 0) {
                // Doc's value is too high, in this dimension
                return false;
            }
        }
    }
    return true;
}

void BKDVisitor::Visit(roaring::Roaring& r) {
    *hits_ |= r;
}

void BKDVisitor::Visit(int doc_id) {
    hits_->add(doc_id);
}

void BKDVisitor::Visit(
    roaring::Roaring* doc_id, 
    std::vector<uint8_t>& packed_value) {
    if (!Matches(packed_value.data())) {
        return;
    }
    Visit(*doc_id);
}

IntersectState::IntersectState(
    FileReader *in,
    int32_t num_dims,
    int32_t packed_bytes_length,
    int32_t packed_index_bytes_length,
    int32_t max_points_in_leaf_node,
    BKDVisitor *visitor,
    const std::shared_ptr<IndexTree> &index_visitor) {
    in_ = std::shared_ptr<FileReader>(in);
    visitor_ = visitor;
    common_prefix_lengths_ = std::vector<int32_t>(num_dims);
    scratch_doc_ids_ = std::vector<int32_t>(max_points_in_leaf_node);
    scratch_data_packed_value_ = std::vector<uint8_t>(packed_bytes_length);
    scratch_min_index_packed_value_ = std::vector<uint8_t>(packed_index_bytes_length);
    scratch_max_index_packed_value_ = std::vector<uint8_t>(packed_index_bytes_length);
    index_ = index_visitor;
}

BKDReader::BKDReader(FileReader *in) {
    in_ = std::unique_ptr<FileReader>(in);
}

std::shared_ptr<IntersectState> 
BKDReader::GetIntersectState(BKDVisitor *visitor) {
    // because we will reuse BKDReader, we need to seek to packed tree index offset every time.
    clone_index_input_->Seek(meta_offset_);
    std::shared_ptr<IndexTree> index;
    if (!packed_index_->empty()) {
        index = std::make_shared<PackedIndexTree>(shared_from_this());
    } else {
        index = std::make_shared<LegacyIndexTree>(shared_from_this());
    }
    return std::make_shared<IntersectState>(in_->Clone(),
                                            num_data_dims_,
                                            packed_bytes_length_,
                                            packed_index_bytes_length_,
                                            max_points_in_leaf_node_,
                                            visitor,
                                            index);
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
    clone_index_input_ = std::shared_ptr<FileReader>(index_in->Clone());
    packed_index_ = std::make_shared<std::vector<uint8_t>>(num_bytes);
    index_in->Read((char*)packed_index_->data(), num_bytes);
    leaf_block_fps_.clear();
    split_packed_values_.clear();
}

void BKDReader::Intersect(
    const std::shared_ptr<IntersectState> &s, 
    std::vector<uint8_t> &cell_min_packed, 
    std::vector<uint8_t> &cell_max_packed) {
    Relation r = s->visitor_->Compare(cell_min_packed, cell_max_packed);

    if (r == Relation::CELL_OUTSIDE_QUERY) {
    } else if (r == Relation::CELL_INSIDE_QUERY) {
        AddAll(s);
    } else if (s->index_->IsLeafNode()) {
        if (s->index_->NodeExists()) {
            int32_t count = BKDUtil::ReadBitmap(s->in_.get(), s->index_->GetLeafBlockFP(), *(s->visitor_->hits_));
        }
    } else {
        int32_t split_dim = s->index_->GetSplitDim();
        assert(split_dim >= 0);
        assert(split_dim < num_index_dims_);

        std::vector<uint8_t> &split_packed_value = s->index_->GetSplitPackedValue();
        std::shared_ptr<BytesRef> split_dim_value = s->index_->GetSplitDimValue();
        assert(split_dim_value->length_ == bytes_per_dim_);
        assert(BKDUtil::CompareUnsigned(cell_min_packed.data(),
                                        split_dim * bytes_per_dim_,
                                        split_dim * bytes_per_dim_ + bytes_per_dim_,
                                        split_dim_value->bytes_.data(),
                                        split_dim_value->offset_,
                                        split_dim_value->offset_ + bytes_per_dim_) <= 0);
        assert(BKDUtil::CompareUnsigned(cell_max_packed.data(),
                                        split_dim * bytes_per_dim_,
                                        split_dim * bytes_per_dim_ + bytes_per_dim_,
                                        split_dim_value->bytes_.data(),
                                        split_dim_value->offset_,
                                        split_dim_value->offset_ + bytes_per_dim_) >= 0);

        std::copy(cell_max_packed.begin(),
                  cell_max_packed.begin() + packed_index_bytes_length_,
                  split_packed_value.begin());
        std::copy(split_dim_value->bytes_.begin() + split_dim_value->offset_,
                  split_dim_value->bytes_.begin() + split_dim_value->offset_ + bytes_per_dim_,
                  split_packed_value.begin() + split_dim * bytes_per_dim_);
        s->index_->PushLeft();
        Intersect(s, cell_min_packed, split_packed_value);
        s->index_->Pop();

        std::copy(split_packed_value.begin() + split_dim * bytes_per_dim_,
                  split_packed_value.begin() + split_dim * bytes_per_dim_ + bytes_per_dim_,
                  split_dim_value->bytes_.begin() + split_dim_value->offset_);

        std::copy(cell_min_packed.begin(),
                  cell_min_packed.begin() + packed_index_bytes_length_,
                  split_packed_value.begin());
        std::copy(split_dim_value->bytes_.begin() + split_dim_value->offset_,
                  split_dim_value->bytes_.begin() + split_dim_value->offset_ + bytes_per_dim_,
                  split_packed_value.begin() + split_dim * bytes_per_dim_);
        s->index_->PushRight();
        Intersect(s, split_packed_value, cell_max_packed);
        s->index_->Pop();
    }
}

void BKDReader::ReadCommonPrefixes(
    std::vector<int32_t> &common_prefix_lengths, 
    std::vector<uint8_t> &scratch_packed_value, 
    FileReader *in) const {
    for (int32_t dim = 0; dim < num_data_dims_; dim++) {
        int32_t prefix = in->ReadVInt();
        common_prefix_lengths[dim] = prefix;
        if (prefix > 0) {
            in->Read((char*)scratch_packed_value.data() +  dim * bytes_per_dim_, prefix);
        }
    }
}

void BKDReader::AddAll(
    const std::shared_ptr<IntersectState> &state) {

    if (state->index_->IsLeafNode()) {
        if (state->index_->NodeExists()) {
            BKDUtil::ReadBitmap(state->in_.get(), state->index_->GetLeafBlockFP(), state->visitor_);
        }
    } else {
        state->index_->PushLeft();
        AddAll(state);
        state->index_->Pop();

        state->index_->PushRight();
        AddAll(state);
        state->index_->Pop();
    }
}

void BKDReader::VisitDocValues(
    std::vector<int32_t> &common_prefix_lengths,
    std::vector<uint8_t> &scratch_data_packed_value,
    const std::vector<uint8_t> &scratch_min_index_packed_value,
    const std::vector<uint8_t> &scratch_max_index_packed_value,
    FileReader *in,
    int32_t count,
    BKDReader *visitor){
    ReadCommonPrefixes(common_prefix_lengths, scratch_data_packed_value, in);
    if(num_index_dims_ != 1){
        for (int32_t dim = 0; dim < num_index_dims_; dim++) {
            int32_t prefix = common_prefix_lengths[dim];
            in->Read((char*)min_packed_value_.data() + (dim * bytes_per_dim_ + prefix), bytes_per_dim_ - prefix);
            in->Read((char*)max_packed_value_.data() + (dim * bytes_per_dim_ + prefix), bytes_per_dim_ - prefix);
        }
    }
}


}