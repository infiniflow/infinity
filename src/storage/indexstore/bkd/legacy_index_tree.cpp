#include "bkd_reader.h"
#include "bkd_writer.h"
#include "legacy_index_tree.h"

namespace infinity {
LegacyIndexTree::LegacyIndexTree(std::shared_ptr<BKDReader>&& reader)
    : IndexTree(reader) {
    split_dim_value_ = std::make_shared<std::vector<uint8_t>>(reader->bytes_per_dim_);
    scratch_ = std::make_shared<BytesRef>();
    SetNodeData();
    scratch_->bytes_ = *split_dim_value_;
    scratch_->length_ = reader->bytes_per_dim_;
}

std::shared_ptr<IndexTree> LegacyIndexTree::Clone() {
    auto index = std::make_shared<LegacyIndexTree>(std::move(reader_));
    index->node_id_ = node_id_;
    index->level_ = level_;
    index->split_dim_ = split_dim_;
    index->leaf_block_fp_ = leaf_block_fp_;
    index->split_packed_value_stack_[index->level_] = split_packed_value_stack_[index->level_];

    return index;
}

void LegacyIndexTree::PushLeft() {
    IndexTree::PushLeft();
    SetNodeData();
}

void LegacyIndexTree::PushRight() {
    IndexTree::PushRight();
    SetNodeData();
}

int64_t LegacyIndexTree::GetLeafBlockFP() {
    assert(IsLeafNode());
    return leaf_block_fp_;
}

std::shared_ptr<BytesRef> LegacyIndexTree::GetSplitDimValue() {
    assert(IsLeafNode() == false);
    return scratch_;
}

void LegacyIndexTree::Pop() {
    IndexTree::Pop();
    leaf_block_fp_ = -1;
}

void LegacyIndexTree::SetNodeData() {
    if (IsLeafNode()) {
        leaf_block_fp_ = reader_->leaf_block_fps_[node_id_ - reader_->leaf_node_offset_];
        split_dim_ = -1;
    } else {
        leaf_block_fp_ = -1;
        int32_t address = node_id_ * reader_->bytes_per_index_entry_;
        if (reader_->num_index_dims_ == 1) {
            split_dim_ = 0;
            if (reader_->version_ < BKDWriter::VERSION_IMPLICIT_SPLIT_DIM_1D) {
                assert(reader_->split_packed_values_[address] == 0);
                address++;
            }
        } else {
            split_dim_ = reader_->split_packed_values_[address++] & 0xff;
        }
        std::copy(reader_->split_packed_values_.begin() + address,
                  reader_->split_packed_values_.begin() + address + reader_->bytes_per_dim_,
                  split_dim_value_->begin());
    }
}


}