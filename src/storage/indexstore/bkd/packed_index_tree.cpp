#include "bkd_reader.h"
#include "packed_index_tree.h"

#include <cassert>

namespace infinity {

PackedIndexTree::PackedIndexTree(std::shared_ptr<BKDReader>&& reader)
    : IndexTree(reader) {
    int32_t tree_depth = reader_->GetTreeDepth();
    leaf_block_fp_stack_ = std::vector<int64_t>(tree_depth + 1);
    left_node_positions_ = std::vector<int32_t>(tree_depth + 1);
    right_node_positions_ = std::vector<int32_t>(tree_depth + 1);
    split_values_stack_ = std::vector<std::shared_ptr<std::vector<uint8_t>>>(tree_depth + 1);
    split_dims_ = std::vector<int32_t>(tree_depth + 1);
    negative_deltas_ = std::vector<bool>(reader_->num_index_dims_ * (tree_depth + 1));

    in_ = reader_->clone_index_input_;
    split_values_stack_[0] = std::make_shared<std::vector<uint8_t>>(reader_->packed_index_bytes_length_);
    ReadNodeData(false);
    scratch_ = std::make_shared<BytesRef>();
    scratch_->length_ = reader_->bytes_per_dim_;
}

std::shared_ptr<IndexTree> PackedIndexTree::Clone() {
    std::shared_ptr<PackedIndexTree> index = std::make_shared<PackedIndexTree>(std::move(reader_));
    index->node_id_ = node_id_;
    index->level_ = level_;
    index->split_dim_ = split_dim_;
    index->leaf_block_fp_stack_[level_] = leaf_block_fp_stack_[level_];
    index->left_node_positions_[level_] = left_node_positions_[level_];
    index->right_node_positions_[level_] = right_node_positions_[level_];
    index->split_values_stack_[index->level_] = split_values_stack_[index->level_];
    std::copy(negative_deltas_.begin() + level_ * reader_->num_index_dims_,
              negative_deltas_.begin() + level_ * reader_->num_index_dims_ + reader_->num_index_dims_,
              index->negative_deltas_.begin() + level_ * reader_->num_index_dims_);
    index->split_dims_[level_] = split_dims_[level_];
    return index;
}

void PackedIndexTree::PushLeft() {
    int32_t node_position = left_node_positions_[level_];
    IndexTree::PushLeft();
    std::copy(negative_deltas_.begin() + (level_ - 1) * reader_->num_index_dims_,
              negative_deltas_.begin() + (level_ - 1) * reader_->num_index_dims_ + reader_->num_index_dims_,
              negative_deltas_.begin() + level_ * reader_->num_index_dims_);
    assert(split_dim_ != -1);
    negative_deltas_[level_ * reader_->num_index_dims_ + split_dim_] = true;
    in_->Seek(node_position);
    ReadNodeData(true);
}

void PackedIndexTree::PushRight() {
    int32_t node_position = right_node_positions_[level_];
    IndexTree::PushRight();
    std::copy(negative_deltas_.begin() + (level_ - 1) * reader_->num_index_dims_,
              negative_deltas_.begin() + (level_ - 1) * reader_->num_index_dims_ + reader_->num_index_dims_,
              negative_deltas_.begin() + level_ * reader_->num_index_dims_);
    assert(split_dim_ != -1);
    negative_deltas_[level_ * reader_->num_index_dims_ + split_dim_] = false;
    in_->Seek(node_position);
    ReadNodeData(false);
}

void PackedIndexTree::Pop() {
    IndexTree::Pop();
    split_dim_ = split_dims_[level_];
}

int64_t PackedIndexTree::GetLeafBlockFP() {
    assert(IsLeafNode());
    return leaf_block_fp_stack_[level_];
}

std::shared_ptr<BytesRef> PackedIndexTree::GetSplitDimValue() {
    assert(IsLeafNode() == false);
    scratch_->bytes_ = *split_values_stack_[level_];
    scratch_->offset_ = split_dim_ * reader_->bytes_per_dim_;
    return scratch_;
}

void PackedIndexTree::ReadNodeData(bool isLeft) {
    leaf_block_fp_stack_[level_] = leaf_block_fp_stack_[level_ - 1];

    if (!isLeft) {
        leaf_block_fp_stack_[level_] += in_->ReadVLong();
    }

    if (IsLeafNode()) {
        split_dim_ = -1;
    } else {
        int32_t code = in_->ReadVInt();
        split_dim_ = code % reader_->num_index_dims_;
        split_dims_[level_] = split_dim_;
        code /= reader_->num_index_dims_;
        int32_t prefix = code % (1 + reader_->bytes_per_dim_);
        int32_t suffix = reader_->bytes_per_dim_ - prefix;

        if (split_values_stack_[level_]==nullptr) {
            split_values_stack_[level_] = std::make_shared<std::vector<uint8_t>>(reader_->packed_index_bytes_length_);
        }
        std::copy(split_values_stack_[level_ - 1]->begin(),
                  split_values_stack_[level_ - 1]->begin() + reader_->packed_index_bytes_length_,
                  split_values_stack_[level_]->begin());
        if (suffix > 0) {
            int32_t first_diff_byte_delta = code / (1 + reader_->bytes_per_dim_);
            if (negative_deltas_[level_ * reader_->num_index_dims_ + split_dim_]) {
                first_diff_byte_delta = -first_diff_byte_delta;
            }
            int32_t oldByte = (*split_values_stack_[level_])[split_dim_ * reader_->bytes_per_dim_ + prefix] & 0xFF;
            (*split_values_stack_[level_])[split_dim_ * reader_->bytes_per_dim_ + prefix] = static_cast<uint8_t>(oldByte + first_diff_byte_delta);
            in_->Read((char*)(split_values_stack_[level_])->data() + (split_dim_ * reader_->bytes_per_dim_ + prefix + 1), suffix - 1);
        } else {

        }

        int32_t left_num_bytes;
        if (node_id_ * 2 < reader_->leaf_node_offset_) {
            left_num_bytes = in_->ReadVInt();
        } else {
            left_num_bytes = 0;
        }

        left_node_positions_[level_] = in_->GetFilePointer();

        right_node_positions_[level_] = left_node_positions_[level_] + left_num_bytes;
    }
}

}