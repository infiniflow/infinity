#include "bkd_reader.h"
#include "index_tree.h"

#include <cassert>

namespace infinity {
IndexTree::IndexTree(std::shared_ptr<BKDReader>& reader)
    : reader_(reader) {
    int32_t tree_depth = reader_->GetTreeDepth();
    split_packed_value_stack_ = std::vector<std::vector<uint8_t>>(tree_depth + 1);
    node_id_ = 1;
    level_ = 1;
    split_packed_value_stack_[level_] = std::vector<uint8_t>(reader_->packed_index_bytes_length_);
}

void IndexTree::PushLeft() {
    node_id_ *= 2;
    level_++;
    if (split_packed_value_stack_[level_].empty()) {
        split_packed_value_stack_[level_] = std::vector<uint8_t>(reader_->packed_index_bytes_length_);
    }
}

void IndexTree::PushRight() {
    node_id_ = node_id_ * 2 + 1;
    level_++;
    if (split_packed_value_stack_[level_].empty()) {
        split_packed_value_stack_[level_] = std::vector<uint8_t>(reader_->packed_index_bytes_length_);
    }
}

void IndexTree::Pop() {
    node_id_ /= 2;
    level_--;
    split_dim_ = -1;
}

bool IndexTree::IsLeafNode() {
    return node_id_ >= reader_->leaf_node_offset_;
}

bool IndexTree::NodeExists() {
    return node_id_ - reader_->leaf_node_offset_ < reader_->leaf_node_offset_;
}

int32_t IndexTree::GetNodeId() {
    return node_id_;
}

std::vector<uint8_t>&IndexTree::GetSplitPackedValue() {
    assert(IsLeafNode() == false);
    assert(!split_packed_value_stack_[level_].empty());
    return split_packed_value_stack_[level_];
}

int32_t IndexTree::GetSplitDim() {
    assert(IsLeafNode() == false);
    return split_dim_;
}

int32_t IndexTree::GetNumLeaves() {
    int32_t left_most_leaf_node = node_id_;
    while (left_most_leaf_node < reader_->leaf_node_offset_) {
        left_most_leaf_node = left_most_leaf_node * 2;
    }
    int32_t right_most_leaf_node = node_id_;
    while (right_most_leaf_node < reader_->leaf_node_offset_) {
        right_most_leaf_node = right_most_leaf_node * 2 + 1;
    }
    int32_t num_leaves;
    if (right_most_leaf_node >= left_most_leaf_node) {
        num_leaves = right_most_leaf_node - left_most_leaf_node + 1;
    } else {
        num_leaves = right_most_leaf_node - left_most_leaf_node + 1 + reader_->leaf_node_offset_;
    }
    assert(num_leaves == GetNumLeavesSlow(node_id_));
    return num_leaves;
}

int32_t IndexTree::GetNumLeavesSlow(int32_t node) {
    if (node >= 2 * reader_->leaf_node_offset_) {
        return 0;
    } else if (node >= reader_->leaf_node_offset_) {
        return 1;
    } else {
        int32_t left_count = GetNumLeavesSlow(node * 2);
        int32_t right_count = GetNumLeavesSlow(node * 2 + 1);
        return left_count + right_count;
    }
}

}