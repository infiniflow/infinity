#include "bkd_writer.h"
#include "bkd_util.h"
#include "msb_radix_sort.h"
#include "common/utility/infinity_assert.h"

#include <numeric>

namespace infinity {

BKDWriter::BKDWriter(
    int32_t max_doc,
    int32_t num_data_dims,
    int32_t num_index_dims,
    int32_t bytes_per_dim,
    int32_t max_points_in_leaf_node,
    double max_points_sort_in_heap,
    int64_t total_point_count,
    bool single_value_per_doc,
    int32_t max_depth)
    : BKDWriter(max_doc, num_data_dims,num_index_dims, bytes_per_dim, max_points_in_leaf_node,
                max_points_sort_in_heap, total_point_count, single_value_per_doc,total_point_count > std::numeric_limits<std::int32_t>::max(), max_depth) {
}

BKDWriter::BKDWriter(
    int32_t max_doc,
    int32_t num_data_dims,
    int32_t num_index_dims,
    int32_t bytes_per_dim,
    int32_t max_points_in_leaf_node,
    double max_points_sort_in_heap,
    int64_t total_point_count,
    bool single_value_per_doc,
    bool long_ords,
    int32_t max_depth) {
    VerifyParams(num_data_dims, num_index_dims, max_points_in_leaf_node, max_points_sort_in_heap, total_point_count);

    max_depth_ = max_depth;
    max_points_in_leaf_node_ = max_points_in_leaf_node;
    num_data_dims_ = num_data_dims;
    num_index_dims_ = num_index_dims;
    bytes_per_dim_ = bytes_per_dim;
    total_point_count_ = total_point_count;
    max_doc_ = max_doc;
    max_depth_ = max_depth;
    packed_bytes_length_ = num_data_dims * bytes_per_dim;
    packed_index_bytes_length_ = num_index_dims_ * bytes_per_dim;
    long_ords_ = long_ords;
    single_value_per_doc_ = single_value_per_doc;

    // dimensional values (num_dims * bytes_per_dim_) + ord (int32_t or int64_t) + docid (int32_t)
    if (single_value_per_doc) {
        // Lucene only supports up to 2.1 docs, so we better not need long_ords in this case:
        assert(long_ords == false);
        bytes_per_doc_ = packed_bytes_length_ + 4;
    } else if (long_ords) {
        bytes_per_doc_ = packed_bytes_length_ + 8 + 4;
    } else {
        bytes_per_doc_ = packed_bytes_length_ + 4 + 4;
    }

    max_points_sort_in_heap_ = total_point_count;
    //max_points_sort_in_heap_ = (int32_t) (0.5 * (max_points_sort_in_heap * 1024 * 1024) / (bytes_per_doc_ * num_data_dims));
    // Finally, we must be able to hold at least the leaf node in heap during build:
    StorageAssert(max_points_sort_in_heap_ >= max_points_in_leaf_node, "max_points_sort_in_heap=" + std::to_string(max_points_sort_in_heap) + " only allows for maxPointsSortInHeap=" + std::to_string(max_points_sort_in_heap_) + ", but this is less than max_points_in_leaf_node=" + std::to_string(max_points_in_leaf_node) + "; either increase max_points_sort_in_heap or decrease max_points_in_leaf_node");
    // max_points_sort_in_heap_ = 100 * 1024 * 1024;
    // We write first maxPointsSortInHeap in heap, then cutover to offline for additional points:
    point_writer_ = std::make_shared<PointWriter>(16, max_points_sort_in_heap_, packed_bytes_length_, long_ords, single_value_per_doc);
    max_mb_sort_in_heap_ = max_points_sort_in_heap;

    max_packed_value_.resize(packed_bytes_length_);
    min_packed_value_.resize(packed_bytes_length_);

    common_prefix_lengths_.resize(num_data_dims_);
    scratch1_.resize(packed_bytes_length_);
    scratch_diff_.resize(bytes_per_dim_);
    scratch2_.resize(packed_bytes_length_);
}

void BKDWriter::VerifyParams(
    int32_t num_data_dims,
    int32_t num_index_dims,
    int32_t max_points_in_leaf_node,
    double max_points_sort_in_heap,
    int64_t total_point_count) {
    std::string msg;
    // We encode dim in a single byte in the split_packed_values, but we only expose 4 bits for it now, in case we want to use
    // remaining 4 bits for another purpose later
    if (num_data_dims < 1 || num_data_dims > MAX_DIMS) {
        msg = "num_data_dims must be 1 .. " + std::to_string(MAX_DIMS) + " (got: " + std::to_string(num_data_dims) + ")";
    }
    if (num_index_dims < 1 || num_index_dims > num_data_dims) {
        msg = "num_index_dims_ must be 1 .. " + std::to_string(num_data_dims) + " (got: " + std::to_string(num_index_dims) + ")";
    }
    if (max_points_in_leaf_node <= 0) {
        msg = "max_points_in_leaf_node must be > 0; got " + std::to_string(max_points_in_leaf_node);
    }
    if (max_points_in_leaf_node > MAX_ARRAY_LENGTH) {
        msg = "max_points_in_leaf_node must be <= MAX_ARRAY_LENGTH (= " + std::to_string(MAX_ARRAY_LENGTH) + "); got " + std::to_string(max_points_in_leaf_node);
    }
    if (max_points_sort_in_heap < 0.0) {
        msg = "max_points_sort_in_heap must be >= 0.0 (got: " + std::to_string(max_points_sort_in_heap) + ")";
    }
    if (total_point_count < 0) {
        msg = "total_point_count must be >=0 (got: " + std::to_string(total_point_count) + ")";
    }
    StorageAssert(msg.empty(), msg);
}


void BKDWriter::CheckMaxLeafNodeCount(int32_t num_leaves) const {
    StorageAssert((1 + bytes_per_dim_) * (int64_t) num_leaves <= MAX_ARRAY_LENGTH, "too many nodes; increase max_points_in_leaf_node");
}

void BKDWriter::Add(
    const uint8_t *packed_value,
    uint32_t value_len,
    int32_t doc_id) {

    StorageAssert(value_len == (size_t) packed_bytes_length_, ("packedValue should be length=" + std::to_string(packed_bytes_length_) + ", got " + std::to_string(value_len)).c_str());
    point_writer_->Append(packed_value, value_len, std::min(point_count_, (int64_t) max_points_sort_in_heap_), doc_id);
    if (point_count_ == 0) {
        memcpy(min_packed_value_.data(), packed_value, packed_index_bytes_length_);
        memcpy(max_packed_value_.data(), packed_value, packed_index_bytes_length_);
    } else {
        for (int32_t dim = 0; dim < num_index_dims_; dim++) {
            int32_t offset = dim * bytes_per_dim_;
            if (BKDUtil::CompareUnsigned(packed_value, offset, offset + bytes_per_dim_, min_packed_value_.data(), offset, offset + bytes_per_dim_) < 0) {
                memcpy(min_packed_value_.data() + offset, packed_value + offset, bytes_per_dim_);
            }
            if (BKDUtil::CompareUnsigned(packed_value, offset, offset + bytes_per_dim_, max_packed_value_.data(), offset, offset + bytes_per_dim_) > 0) {
                memcpy(max_packed_value_.data() + offset, packed_value + offset, bytes_per_dim_);
            }
        }
    }

    point_count_++;
    StorageAssert(point_count_ <= total_point_count_, ("total_point_count_=" + std::to_string(total_point_count_) + " was passed when we were created, but we just hit " + std::to_string(point_count_) + " values").c_str());
}

void BKDWriter::Add(
    std::vector<uint8_t> &packed_value,
    int32_t docid) {

    StorageAssert(packed_value.size() == (size_t) packed_bytes_length_, ("packed_value should be length=" + std::to_string(packed_bytes_length_) + ", got " + std::to_string(packed_value.size())).c_str());
    point_writer_->Append(packed_value, std::min(point_count_, (int64_t) max_points_sort_in_heap_), docid);

    if (point_count_ == 0) {
        std::copy(packed_value.begin(),
                  packed_value.begin() + packed_index_bytes_length_,
                  min_packed_value_.begin());
        std::copy(packed_value.begin(),
                  packed_value.begin() + packed_index_bytes_length_,
                  max_packed_value_.begin());
    } else {
        for (int32_t dim = 0; dim < num_index_dims_; dim++) {
            int32_t offset = dim * bytes_per_dim_;
            if (BKDUtil::CompareUnsigned(packed_value.data(), offset, offset + bytes_per_dim_, min_packed_value_.data(), offset, offset + bytes_per_dim_) < 0) {
                std::copy(packed_value.begin() + offset,
                          packed_value.begin() + offset + bytes_per_dim_,
                          min_packed_value_.begin() + offset);
            }
            if (BKDUtil::CompareUnsigned(packed_value.data(), offset, offset + bytes_per_dim_, max_packed_value_.data(), offset, offset + bytes_per_dim_) > 0) {
                std::copy(packed_value.begin() + offset,
                          packed_value.begin() + offset + bytes_per_dim_,
                          max_packed_value_.begin() + offset);
            }
        }
    }

    point_count_++;
    StorageAssert(point_count_ <= total_point_count_, ("total_point_count_=" + std::to_string(total_point_count_) + " was passed when we were created, but we just hit " + std::to_string(point_count_) + " values").c_str());

}

void BKDWriter::MetaFinish(
    FileWriter *out,
    int64_t fPointer,
    int32_t type) {
    out->WriteInt(type);
    out->WriteLong(fPointer);
}

int64_t BKDWriter::Finish(
    FileWriter *out,
    FileWriter *index_out) {
    if (point_count_ == 0) {
        return 0;
    }
    std::shared_ptr<BitSet> ord_bit_set = nullptr;
    if (num_index_dims_ > 1) {
        if (single_value_per_doc_)
            ord_bit_set = std::make_shared<BitSet>(max_doc_);
        else
            ord_bit_set = std::make_shared<BitSet>(point_count_);
    }

    auto max_points_in_leaf_node = point_count_ / (1LL << max_depth_);
    if (max_points_in_leaf_node > 0) {
        max_points_in_leaf_node_ = max_points_in_leaf_node;
    }

    int64_t count_per_leaf = point_count_;
    int64_t inner_node_count = 1;
    while (count_per_leaf > max_points_in_leaf_node_) {
        count_per_leaf = (count_per_leaf + 1) / 2;
        inner_node_count *= 2;
    }
    auto num_leaves = (int32_t) inner_node_count;
    CheckMaxLeafNodeCount(num_leaves);
    // Indexed by node_id, but first (root) node_id is 1.  We do 1+ because the lead byte at each recursion says which dim we split on.
    std::vector<uint8_t> split_packed_values(num_leaves * (1 + bytes_per_dim_));
    std::vector<int64_t> leaf_block_fps(num_leaves);
    std::vector<std::shared_ptr<PathSlice>> sorted_point_writers(num_index_dims_);
    std::vector<std::shared_ptr<PointReader>> to_close_heroically;
    bool success = false;
    std::vector<int32_t> parent_splits(num_index_dims_);
    Build(1, num_leaves, sorted_point_writers,
          ord_bit_set, out,
          min_packed_value_, max_packed_value_,
          parent_splits,
          split_packed_values,
          leaf_block_fps,
          to_close_heroically);
    int64_t index_fp = out->TotalWrittenBytes();
    WriteIndex(index_out, count_per_leaf, leaf_block_fps, split_packed_values);
    return index_fp;
}

void BKDWriter::WriteLeafBlockDocs(
    FileWriter *out,
    std::vector<int32_t> &docids,
    int32_t start,
    int32_t count) {
    assert(count > 0);
    out->WriteVInt(count);
    BKDUtil::WriteDocIds(docids, start, count, out);
}

void BKDWriter::WriteLeafBlockDocsBitmap(
    FileWriter *out,
    std::vector<int32_t> &docids,
    int32_t start,
    int32_t count) {
    assert(count > 0);
    out->WriteVInt(count);
    out->WriteByte((uint8_t) 1);
    BKDUtil::WriteDocIdsBitmap(docids, start, count, out);
}

void BKDWriter::WriteCommonPrefixes(
    FileWriter *out,
    std::vector<int32_t> &common_prefixes,
    std::vector<uint8_t> &packed_value) const {
    for (int32_t dim = 0; dim < num_data_dims_; dim++) {
        out->WriteVInt(common_prefixes[dim]);
        out->Write((char*)packed_value.data() + dim * bytes_per_dim_, common_prefixes[dim]);
    }
}

void BKDWriter::WriteCommonPrefixes(
    FileWriter *out,
    std::vector<int32_t> &common_prefixes,
    std::vector<uint8_t> &packed_value,
    int offset) const {
    for (int32_t dim = 0; dim < num_data_dims_; dim++) {
        out->WriteVInt(common_prefixes[dim]);
        out->Write((char*)packed_value.data() + (dim * bytes_per_dim_ + offset), common_prefixes[dim]);
    }
}

int64_t BKDWriter::GetLeftMostLeafBlockFP(
    std::vector<int64_t> &leaf_block_fps,
    int32_t nodeID) {
    while (nodeID < (int32_t)leaf_block_fps.size())
        nodeID *= 2;
    int32_t leaf_id = nodeID - leaf_block_fps.size();
    int64_t result = leaf_block_fps.at(leaf_id);
    StorageAssert(result >= 0, "file pointer less than 0");
    return result;
}

int32_t BKDWriter::AppendBlock(
    const std::shared_ptr<WriteBuffer>& write_buffer,
    ByteArrayList &blocks) {
    std::vector<uint8_t> bytes = std::vector<uint8_t>(write_buffer->Count());
    write_buffer->WriteTo(reinterpret_cast<char*>(bytes.data()), write_buffer->Count());
    write_buffer->Reset();
    blocks.emplace_back(bytes);
    return write_buffer->Count();
}

int32_t BKDWriter::RecursePackIndex(
    const std::shared_ptr<WriteBuffer> &write_buffer,
    std::vector<int64_t> &leaf_block_fps,
    std::vector<uint8_t> &split_packed_values,
    int64_t min_block_fp,
    ByteArrayList &blocks,
    int32_t nodeID,
    std::vector<uint8_t> &last_split_values,
    std::vector<bool> &negative_deltas,
    bool isLeft) const {
    if (nodeID >= (int32_t) leaf_block_fps.size()) {
        int32_t leaf_id = nodeID - leaf_block_fps.size();
        if (leaf_id < (int32_t) leaf_block_fps.size()) {
            int64_t delta = leaf_block_fps.at(leaf_id) - min_block_fp;
            if (isLeft) {
                assert(delta == 0);
                return 0;
            } else {
                assert(nodeID == 1 || delta > 0);
                write_buffer->WriteVLong(delta);
                return AppendBlock(write_buffer, blocks);
            }
        } else {
            return 0;
        }
    } else {
        int64_t left_block_fp;
        if (!isLeft) {
            left_block_fp = GetLeftMostLeafBlockFP(leaf_block_fps, nodeID);
            int64_t delta = left_block_fp - min_block_fp;
            assert(nodeID == 1 || delta > 0);
            write_buffer->WriteVLong(delta);
        } else {
            left_block_fp = min_block_fp;
        }

        int32_t address = nodeID * (1 + bytes_per_dim_);
        int32_t split_dim = split_packed_values[address++] & 0xff;

        int32_t prefix = BKDUtil::Mismatch(split_packed_values, address, address + bytes_per_dim_,
                                           last_split_values, split_dim * bytes_per_dim_, split_dim * bytes_per_dim_ + bytes_per_dim_);
        if (prefix == -1)
            prefix = bytes_per_dim_;

        int32_t first_diff_byte_delta;
        if (prefix < bytes_per_dim_) {
            first_diff_byte_delta = (split_packed_values[address + prefix] & 0xff) -
                                    (last_split_values[split_dim * bytes_per_dim_ + prefix] & 0xff);
            if (negative_deltas[split_dim])
                first_diff_byte_delta = -first_diff_byte_delta;
            assert(first_diff_byte_delta > 0);

        } else {
            first_diff_byte_delta = 0;
        }
        int32_t code = (first_diff_byte_delta * (1 + bytes_per_dim_) + prefix) * num_index_dims_ + split_dim;
        write_buffer->WriteVInt(code);
        int32_t suffix = bytes_per_dim_ - prefix;
        std::vector<uint8_t> save_split_value(suffix);
        if (suffix > 1)
            write_buffer->Write((char*)split_packed_values.data() + (address + prefix + 1), suffix - 1);

        std::vector<uint8_t> cmp = last_split_values;

        std::copy(last_split_values.begin() + split_dim * bytes_per_dim_ + prefix,
                  last_split_values.begin() + split_dim * bytes_per_dim_ + prefix + suffix,
                  save_split_value.begin());

        std::copy(split_packed_values.begin() + address + prefix,
                  split_packed_values.begin() + address + prefix + suffix,
                  last_split_values.begin() + split_dim * bytes_per_dim_ + prefix);
        int32_t num_bytes = AppendBlock(write_buffer, blocks);

        int32_t idx_save = blocks.size();
        blocks.emplace_back(std::vector<uint8_t>(0));
        bool save_negative_delta = negative_deltas[split_dim];
        negative_deltas[split_dim] = true;
        int32_t left_num_bytes = RecursePackIndex(write_buffer, leaf_block_fps, split_packed_values, left_block_fp, blocks,
                                 2 * nodeID, last_split_values, negative_deltas, true);
        if (nodeID * 2 < (int32_t) leaf_block_fps.size())
            write_buffer->WriteVInt(left_num_bytes);
        else
            assert(left_num_bytes == 0);

        int32_t num_bytes2 = write_buffer->Count();
        std::vector<uint8_t> bytes2(num_bytes2);
        write_buffer->WriteTo(reinterpret_cast<char*>(bytes2.data()), num_bytes2);
        write_buffer->Reset();
        blocks[idx_save] = std::move(bytes2);

        negative_deltas[split_dim] = false;
        int32_t right_num_bytes = RecursePackIndex(write_buffer, leaf_block_fps, split_packed_values, left_block_fp, blocks,
                                  2 * nodeID + 1, last_split_values, negative_deltas, false);
        negative_deltas[split_dim] = save_negative_delta;
        std::copy(save_split_value.begin(),
                  save_split_value.begin() + suffix,
                  last_split_values.begin() + split_dim * bytes_per_dim_ + prefix);

        assert(std::equal(last_split_values.begin(), last_split_values.end(), cmp.begin()));
        return num_bytes + num_bytes2 + left_num_bytes + right_num_bytes;
    }
}

std::shared_ptr<std::vector<uint8_t>>
BKDWriter::PackIndex(
    std::vector<int64_t> &leaf_block_fps,
    std::vector<uint8_t> &split_packed_values) const {
    int32_t num_leaves = leaf_block_fps.size();
    if (num_index_dims_ == 1 && num_leaves > 1) {
        int32_t level_count = 2;
        while (true) {
            if (num_leaves >= level_count && num_leaves <= 2 * level_count) {
                int32_t last_level = 2 * (num_leaves - level_count);
                assert(last_level >= 0);
                if (last_level != 0) {
                    std::vector<int64_t> new_leaf_block_fps(num_leaves);
                    std::copy(leaf_block_fps.begin() + last_level,
                              leaf_block_fps.begin() + last_level + (leaf_block_fps.size() - last_level),
                              new_leaf_block_fps.begin());
                    std::copy(leaf_block_fps.begin(),
                              leaf_block_fps.begin() + last_level,
                              new_leaf_block_fps.begin() + (leaf_block_fps.size() - last_level));
                    leaf_block_fps.swap(new_leaf_block_fps);
                }
                break;
            }
            level_count *= 2;
        }
    }
    std::shared_ptr<WriteBuffer> write_buffer = std::make_shared<WriteBuffer>();
    ByteArrayList blocks;
    std::vector<uint8_t> last_split_values(bytes_per_dim_ * num_index_dims_);
    std::vector<bool> bools(num_index_dims_);
    int32_t total_size = RecursePackIndex(write_buffer, leaf_block_fps, split_packed_values, (int64_t) 0, blocks, 1, last_split_values, bools, false);
    std::shared_ptr<std::vector<uint8_t>> index = std::make_shared<std::vector<uint8_t>>(total_size);
    int32_t upto = 0;
    for (auto &block: blocks) {
        std::copy(block.begin(), block.begin() + block.size(), index->begin() + upto);
        upto += block.size();
    }
    assert(upto == total_size);
    return index;
}

void BKDWriter::WriteIndex(
    FileWriter *out, 
    int32_t count_per_leaf, 
    std::vector<int64_t> &leaf_block_fps,
    std::vector<uint8_t> &split_packed_values) {
    std::shared_ptr<std::vector<uint8_t>> packed_index = PackIndex(leaf_block_fps, split_packed_values);
    WriteIndex(out, count_per_leaf, leaf_block_fps.size(), *packed_index);
}

void BKDWriter::WriteIndex(
    FileWriter *out,
    int32_t count_per_leaf,
    int32_t num_leaves,
    std::vector<uint8_t> &packed_index) {

    out->WriteVInt(num_data_dims_);
    out->WriteVInt(num_index_dims_);
    out->WriteVInt(count_per_leaf);
    out->WriteVInt(bytes_per_dim_);

    assert(num_leaves > 0);
    out->WriteVInt(num_leaves);
    out->Write((char_t*)min_packed_value_.data(), packed_index_bytes_length_);
    out->Write((char_t*)max_packed_value_.data(), packed_index_bytes_length_);

    out->WriteVLong(point_count_);
    out->WriteVInt(docs_seen_);
    out->WriteVInt(packed_index.size());

    out->Write((char_t*)packed_index.data(), packed_index.size());
}

int32_t BKDWriter::RunLen(
    const IntFunction<BytesRef *> &packed_values,
    int32_t start,
    int32_t end,
    int32_t byte_offset) {
    BytesRef *first = packed_values(start);
    uint8_t b = first->bytes_.at(first->offset_ + byte_offset);
    for (int32_t i = start + 1; i < end; ++i) {
        BytesRef *ref = packed_values(i);
        uint8_t b2 = ref->bytes_.at(ref->offset_ + byte_offset);
        if (b != b2)
            return i - start;
    }
    return end - start;
}

void BKDWriter::WriteLeafBlockPackedValuesRange(
    FileWriter *out,
    std::vector<int32_t> &commonPrefixLengths,
    int32_t start,
    int32_t end,
    const IntFunction<BytesRef *> &packedValues) const {
    for (int32_t i = start; i < end; i++) {
        BytesRef *ref = packedValues(i);
        assert(ref->length_ == packed_bytes_length_);
        for (int32_t dim = 0; dim < num_data_dims_; dim++) {
            int32_t prefix = commonPrefixLengths[dim];
            out->Write((char*)ref->bytes_.data() + (ref->offset_ + dim * bytes_per_dim_ + prefix), bytes_per_dim_ - prefix);
        }
    }
}

void BKDWriter::WriteLowCardinalityLeafBlockPackedValues(
    FileWriter *out,
    std::vector<int32_t> &common_prefix_lengths,
    int32_t count,
    const IntFunction<BytesRef *> &packed_values) {

    BytesRef *value = packed_values(0);
    std::copy(value->bytes_.begin() + value->offset_, value->bytes_.begin() + value->offset_ + num_data_dims_ * bytes_per_dim_, scratch1_.begin());
    int cardinality = 1;
    for (int i = 1; i < count; i++) {
        value = packed_values(i);
        for (int dim = 0; dim < num_data_dims_; dim++) {
            int start = dim * bytes_per_dim_ + common_prefix_lengths[dim];
            int end = dim * bytes_per_dim_ + bytes_per_dim_;
            if (BKDUtil::Mismatch(value->bytes_, value->offset_ + start, value->offset_ + end, scratch1_, start, end) != -1) {
                out->WriteVInt(cardinality);
                for (int j = 0; j < num_data_dims_; j++) {
                    out->Write((char*)scratch1_.data() + (j * bytes_per_dim_ + common_prefix_lengths[j]), bytes_per_dim_ - common_prefix_lengths[j]);
                }
                std::copy(value->bytes_.begin() + value->offset_, value->bytes_.begin() + value->offset_ + num_data_dims_ * bytes_per_dim_, scratch1_.begin());
                cardinality = 1;
                break;
            } else if (dim == num_data_dims_ - 1) {
                cardinality++;
            }
        }
    }
    out->WriteVInt(cardinality);
    for (int i = 0; i < num_data_dims_; i++) {
        out->Write((char*)scratch1_.data() + (i * bytes_per_dim_ + common_prefix_lengths[i]), bytes_per_dim_ - common_prefix_lengths[i]);
    }
}

void BKDWriter::WriteHighCardinalityLeafBlockPackedValues(
    FileWriter *out,
    std::vector<int32_t> &common_prefix_lengths,
    int32_t count,
    int32_t sorted_dim,
    const IntFunction<BytesRef *> &packed_values,
    int32_t compressed_byte_offset) const {
    common_prefix_lengths[sorted_dim]++;

    for (int32_t i = 0; i < count;) {
        int32_t run_length = RunLen(packed_values, i, std::min(i + 0xff, count), compressed_byte_offset);
        assert(run_length <= 0xff);
        BytesRef *first = packed_values(i);
        uint8_t prefix_byte = first->bytes_.at(first->offset_ + compressed_byte_offset);
        out->WriteByte(prefix_byte);
        out->WriteByte((uint8_t) run_length);
        WriteLeafBlockPackedValuesRange(out, common_prefix_lengths, i, i + run_length, packed_values);
        i += run_length;
        assert(i <= count);
    }
}

void BKDWriter::WriteLeafBlockPackedValues(
    FileWriter *out,
    std::vector<int32_t> &common_prefix_lengths,
    int32_t count,
    int32_t sorted_dim,
    const IntFunction<BytesRef *> &packed_values,
    int32_t prefix_len_sum,
    bool low_cardinality) {
    if (prefix_len_sum == packed_bytes_length_) {
        out->WriteByte(int8_t(-1));
    } else {
        int32_t compressed_byte_offset = sorted_dim * bytes_per_dim_ + common_prefix_lengths[sorted_dim];
        if (low_cardinality) {
            out->WriteByte(int8_t(-2));
            WriteLowCardinalityLeafBlockPackedValues(out, common_prefix_lengths, count, packed_values);
        } else {
            out->WriteByte((uint8_t) sorted_dim);
            WriteHighCardinalityLeafBlockPackedValues(out, common_prefix_lengths, count, sorted_dim, packed_values, compressed_byte_offset);
        }
    }
}

int32_t BKDWriter::Split(
    std::vector<uint8_t> &min_packed_value,
    std::vector<uint8_t> &max_packed_value,
    std::vector<int32_t> &parent_splits) {
    int32_t max_num_splits = 0;
    max_num_splits = *(std::max_element(parent_splits.begin(), parent_splits.end()));
    for (int32_t dim = 0; dim < num_index_dims_; ++dim) {
        int32_t offset = dim * bytes_per_dim_;
        if (parent_splits[dim] < max_num_splits / 2 && BKDUtil::CompareUnsigned(min_packed_value.data(), offset, offset + bytes_per_dim_, max_packed_value.data(), offset, offset + bytes_per_dim_) != 0)
            return dim;
    }
    int32_t split_dim = -1;
    for (int32_t dim = 0; dim < num_index_dims_; dim++) {
        BKDUtil::Subtract(bytes_per_dim_, dim, max_packed_value, min_packed_value, scratch_diff_);
        if (split_dim == -1 || BKDUtil::CompareUnsigned(scratch_diff_.data(), 0, bytes_per_dim_, scratch1_.data(), 0, bytes_per_dim_) > 0) {
            std::copy(scratch_diff_.begin(), scratch_diff_.begin() + bytes_per_dim_, scratch1_.begin());
            split_dim = dim;
        }
    }
    return split_dim;
}

const uint8_t *BKDWriter::MarkRightTree(
    int64_t right_count,
    int32_t split_dim,
    std::shared_ptr<PathSlice> &source,
    const std::shared_ptr<BitSet> &ord_bitset) const {
    std::shared_ptr<PointReader> reader = source->writer_->GetPointReader(source->start_ + source->count_ - right_count, right_count);
    bool result = reader->Next();
    assert(result);
    if (reader != nullptr) {
        if (num_index_dims_ > 1 && ord_bitset != nullptr) {
            assert(ord_bitset->Get(reader->Ord()) == false);
            ord_bitset->Set(reader->Ord());
            reader->MarkOrds(right_count - 1, ord_bitset);
        }
        return reader->PackedValueRaw() + split_dim * bytes_per_dim_;
    }
    return nullptr;
}

std::shared_ptr<PointWriter>
BKDWriter::GetPointWriter(int64_t count, std::string &desc) {
    return std::make_shared<PointWriter>(std::min(count, int64_t(max_points_sort_in_heap_)), std::min(count, int64_t(max_points_sort_in_heap_)), packed_bytes_length_, long_ords_, single_value_per_doc_);
}

bool BKDWriter::LeafNodeIsLowCardinality(
    std::vector<int32_t> &common_prefix_lengths,
    int32_t count,
    int32_t sorted_dim,
    const IntFunction<BytesRef *> &packed_values,
    const std::vector<int> &leaf_cardinality,
    int32_t prefix_len_sum) const {
    int32_t compressed_byte_offset = sorted_dim * bytes_per_dim_ + common_prefix_lengths[sorted_dim];
    int high_cardinality_cost;
    int low_cardinality_cost;
    if (count == leaf_cardinality.size()) {
        high_cardinality_cost = 0;
        low_cardinality_cost = 1;
    } else {
        int numRunLens = 0;
        for (int i = 0; i < count;) {
            int32_t run_length = RunLen(packed_values, i, std::min(i + 0xff, count), compressed_byte_offset);
            assert(run_length <= 0xff);
            numRunLens++;
            i += run_length;
        }
        high_cardinality_cost = count * (num_data_dims_ * bytes_per_dim_ - prefix_len_sum - 1) + 2 * numRunLens;
        low_cardinality_cost = leaf_cardinality.size() * (num_data_dims_ * bytes_per_dim_ - prefix_len_sum + 1);
    }
    return (low_cardinality_cost <= high_cardinality_cost);
}

void BKDWriter::CreatePathSlice(
    std::vector<std::shared_ptr<PathSlice>> &slices,
    int32_t dim) {
    assert(slices[dim] == nullptr);
    std::shared_ptr<PathSlice> current = nullptr;
    for (const std::shared_ptr<PathSlice> &slice: slices) {
        if (slice != nullptr) {
            current = slice;
            break;
        }
    }
    if (current == nullptr) {
        slices[dim] = std::make_shared<PathSlice>(Sort(dim), 0, point_count_);
    } else {
        slices[dim] = std::make_shared<PathSlice>(Sort(dim, current->writer_, current->start_, current->count_), 0, current->count_);
    }
}

std::shared_ptr<PointWriter> BKDWriter::CreatePointWriterCopy(
    const std::shared_ptr<PointWriter> &writer,
    int64_t start,
    int64_t count) {
    int32_t size = count;
    std::shared_ptr<PointWriter> point_writer = std::make_shared<PointWriter>(size, size, packed_bytes_length_, long_ords_, single_value_per_doc_);
    std::shared_ptr<PointReader> reader = writer->GetPointReader(start, count);
    if (point_writer != nullptr && reader != nullptr) {
        for (int64_t i = 0; i < count; i++) {
            reader->Next();
            point_writer->Append(reader->PackedValueRaw(), packed_bytes_length_, reader->Ord(), reader->Docid());
        }
        return point_writer;
    }
    throw StorageException("point index corrupt");
}

std::shared_ptr<PointWriter> BKDWriter::Sort(
    int32_t dim,
    const std::shared_ptr<PointWriter> &writer,
    int64_t start,
    int64_t point_count) {
    assert(dim >= 0 && dim < num_data_dims_);
    auto w = std::dynamic_pointer_cast<PointWriter>(writer);
    if (w != nullptr) {
        std::shared_ptr<PointWriter> point_writer = CreatePointWriterCopy(w, start, point_count);
        SortPointWriter(point_writer, point_count, dim);
        return point_writer;
    }
    return nullptr;
}

std::shared_ptr<PointWriter> BKDWriter::Sort(int32_t dim) {
    assert(dim >= 0 && dim < num_data_dims_);
    if (point_writer_ != nullptr) {
        std::shared_ptr<PointWriter> sorted = std::move(point_writer_);
        SortPointWriter(sorted, point_count_, dim);
        sorted->Close();
        point_writer_ = nullptr;
        return std::dynamic_pointer_cast<PointWriter>(sorted);
    }
    return nullptr;
}

void BKDWriter::SortPointWriter(
    std::shared_ptr<PointWriter> &writer,
    int32_t point_count,
    int32_t dim) {
    auto sorter = std::make_shared<MSBRadixSorter>(shared_from_this().get(), writer.get(), dim, bytes_per_dim_ + 4);
    sorter->Sort(0, point_count);
}

std::shared_ptr<BKDWriter::PathSlice> BKDWriter::SwitchToHeap(
    const std::shared_ptr<PathSlice> &source,
    const std::vector<std::shared_ptr<PointReader>> &to_close_heroically) {
    auto count = (int32_t) (source->count_);
    std::shared_ptr<PointReader> reader = source->writer_->GetPointReader(source->start_, source->count_, to_close_heroically);
    std::shared_ptr<PointWriter> writer = std::make_shared<PointWriter>(count, count, packed_bytes_length_, long_ords_, single_value_per_doc_);
    if (writer != nullptr) {
        for (int32_t i = 0; i < count; i++) {
            bool has_next = reader->Next();
            assert(has_next);
            writer->Append(reader->PackedValueRaw(), packed_bytes_length_, reader->Ord(), reader->Docid());
        }
    }
    return std::make_shared<PathSlice>(writer, 0, count);
}


void BKDWriter::Build(
    int32_t nodeID,
    int32_t leaf_node_offset,
    std::vector<std::shared_ptr<PathSlice>> &slices,
    const std::shared_ptr<BitSet> &ord_bitset,
    FileWriter *out,
    std::vector<uint8_t> &min_packed_value,
    std::vector<uint8_t> &max_packed_value,
    std::vector<int32_t> &parent_splits,
    std::vector<uint8_t> &split_packed_values,
    std::vector<int64_t> &leaf_block_fps,
    const std::vector<std::shared_ptr<PointReader>> &to_close_heroically) {
    if (nodeID >= leaf_node_offset) {

        int32_t sorted_dim = 0;
        int32_t sorted_dim_cardinality = std::numeric_limits<int32_t>::max();

        for (int32_t dim = 0; dim < num_index_dims_; dim++) {
            bool created = false;
            if (slices[dim] == nullptr) {
                CreatePathSlice(slices, dim);
                created = true;
            }
            if (std::dynamic_pointer_cast<PointWriter>(slices[dim]->writer_) == nullptr) {
                std::shared_ptr<PathSlice> slice = slices[dim];
                slices[dim] = SwitchToHeap(slices[dim], to_close_heroically);
                if (created) {
                    slice->writer_->Destroy();
                }
            }

            std::shared_ptr<PathSlice> source = slices[dim];

            std::shared_ptr<PointWriter> heap_source = std::dynamic_pointer_cast<PointWriter>(source->writer_);

            heap_source->ReadPackedValue(source->start_, scratch1_);
            heap_source->ReadPackedValue(source->start_ + source->count_ - 1, scratch2_);
            int32_t offset = dim * bytes_per_dim_;
            common_prefix_lengths_[dim] = BKDUtil::Mismatch(scratch1_, offset, offset + bytes_per_dim_, scratch2_, offset, offset + bytes_per_dim_);

            if (common_prefix_lengths_[dim] == -1)
                common_prefix_lengths_[dim] = bytes_per_dim_;
            sorted_dim = dim;
        }

        std::shared_ptr<PathSlice> data_dim_path_slice = nullptr;

        if (num_data_dims_ != num_index_dims_) {
            std::shared_ptr<PointWriter> heap_source = std::dynamic_pointer_cast<PointWriter>(slices[0]->writer_);
            auto from = (int32_t) slices[0]->start_;
            int32_t to = from + (int32_t) slices[0]->count_;
            std::fill(common_prefix_lengths_.begin() + num_index_dims_,
                      common_prefix_lengths_.begin() + num_index_dims_ + num_data_dims_,
                      bytes_per_dim_);
            heap_source->ReadPackedValue(from, scratch1_);
            for (int32_t i = from + 1; i < to; ++i) {
                heap_source->ReadPackedValue(i, scratch2_);
                for (int32_t dim = num_index_dims_; dim < num_data_dims_; dim++) {
                    int32_t offset = dim * bytes_per_dim_;
                    int32_t dimension_prefix_length = common_prefix_lengths_[dim];
                    common_prefix_lengths_[dim] = BKDUtil::Mismatch(scratch1_, offset, offset + dimension_prefix_length,
                                                  scratch2_, offset, offset + dimension_prefix_length);
                    if (common_prefix_lengths_[dim] == -1) {
                        common_prefix_lengths_[dim] = dimension_prefix_length;
                    }
                }
            }
            if (common_prefix_lengths_[sorted_dim] == bytes_per_dim_) {
                for (int32_t dim = num_index_dims_; dim < num_data_dims_; ++dim) {
                    if (common_prefix_lengths_[dim] != bytes_per_dim_) {
                        sorted_dim = dim;
                        data_dim_path_slice = SwitchToHeap(slices[0], to_close_heroically);
                        std::shared_ptr<PointWriter> heap_writer = std::dynamic_pointer_cast<PointWriter>(data_dim_path_slice->writer_);
                        SortPointWriter(heap_writer, (int32_t) data_dim_path_slice->count_, sorted_dim);
                        break;
                    }
                }
            }
        }

        std::shared_ptr<PathSlice> source = (data_dim_path_slice != nullptr) ? data_dim_path_slice : slices[sorted_dim];

        std::shared_ptr<PointWriter> heap_source = std::dynamic_pointer_cast<PointWriter>(source->writer_);
        auto from = (int32_t) slices[0]->start_;
        int32_t to = from + (int32_t) slices[0]->count_;
        auto leaf_cardinality = heap_source->ComputeCardinality(from, to, num_data_dims_, bytes_per_dim_, common_prefix_lengths_);

        leaf_block_fps[nodeID - leaf_node_offset] = out->TotalWrittenBytes();
        int32_t count = source->count_;
        scratch_bytes_ref2.length_ = packed_bytes_length_;
        std::function<BytesRef *(int32_t)> packed_values = [&](int32_t i) -> BytesRef * {
            heap_source->GetPackedValueSlice((source->start_ + i), scratch_bytes_ref2);
            return &scratch_bytes_ref2;
        };
        assert(count > 0);
        int32_t prefix_len_sum = std::accumulate(common_prefix_lengths_.begin(), common_prefix_lengths_.end(), 0);
        bool low_cardinal = false;
        if (prefix_len_sum == packed_bytes_length_) {
            WriteLeafBlockDocsBitmap(out, heap_source->doc_IDs_, source->start_, count);
        } else {
            low_cardinal = LeafNodeIsLowCardinality(common_prefix_lengths_, count, sorted_dim, packed_values, leaf_cardinality, prefix_len_sum);
            WriteLeafBlockDocs(out, heap_source->doc_IDs_, source->start_, count);
        }

        WriteCommonPrefixes(out, common_prefix_lengths_, scratch1_);

        WriteLeafBlockPackedValues(out, common_prefix_lengths_, count, sorted_dim, packed_values, prefix_len_sum, low_cardinal);
    } else {
        int32_t split_dim;
        if (num_index_dims_ > 1) {
            //TODO
        } else {
            split_dim = 0;
        }

        bool delete_split_dim = false;
        if (slices[split_dim] == nullptr) {
            CreatePathSlice(slices, split_dim);
            delete_split_dim = true;
        }
        std::shared_ptr<PathSlice> source = slices[split_dim];

        assert(nodeID < (int32_t) split_packed_values.size());

        int64_t right_count = source->count_ / 2;
        int64_t left_count = source->count_ - right_count;

        int32_t dim_to_clear = num_index_dims_ - 1;
        while (dim_to_clear >= 0) {
            if (slices[dim_to_clear] != nullptr && split_dim != dim_to_clear) {
                break;
            }
            dim_to_clear--;
        }

        auto split_value = (dim_to_clear == -1) ? MarkRightTree(right_count, split_dim, source, nullptr) : MarkRightTree(right_count, split_dim, source, ord_bitset);
        StorageAssert(split_value != nullptr, "split value pointer is null");
        int32_t address = nodeID * (1 + bytes_per_dim_);
        split_packed_values[address] = (uint8_t) split_dim;
        std::copy(split_value, split_value + bytes_per_dim_, split_packed_values.begin() + address + 1);

        std::vector<std::shared_ptr<PathSlice>> left_slices(num_index_dims_);
        std::vector<std::shared_ptr<PathSlice>> right_slices(num_index_dims_);

        std::vector<uint8_t> min_split_packed_value(packed_index_bytes_length_);
        std::copy(min_packed_value.begin(), min_packed_value.begin() + packed_index_bytes_length_, min_split_packed_value.begin());

        std::vector<uint8_t> max_split_packed_value(packed_index_bytes_length_);
        std::copy(max_packed_value.begin(), max_packed_value.begin() + packed_index_bytes_length_, max_split_packed_value.begin());


        for (int32_t dim = 0; dim < num_index_dims_; dim++) {
            if (slices[dim] == nullptr) {
                continue;
            }
            if (dim == split_dim) {
                left_slices[dim] = std::make_shared<PathSlice>(source->writer_, source->start_, left_count);
                right_slices[dim] = std::make_shared<PathSlice>(source->writer_, source->start_ + left_count, right_count);
                std::copy(split_value, split_value + bytes_per_dim_, min_split_packed_value.begin() + dim * bytes_per_dim_);
                std::copy(split_value, split_value + bytes_per_dim_, max_split_packed_value.begin() + dim * bytes_per_dim_);
                continue;
            }

            std::shared_ptr<PointReader> reader = slices[dim]->writer_->GetPointReader(slices[dim]->start_, slices[dim]->count_, to_close_heroically);
            std::string desc = "left" + std::to_string(dim);
            std::shared_ptr<PointWriter> left_point_writer = GetPointWriter(left_count, desc);
            if (left_point_writer != nullptr) {
                desc = "right" + std::to_string(dim);
                std::shared_ptr<PointWriter> right_point_writer = GetPointWriter(source->count_ - left_count, desc);

                int64_t next_right_count = reader->Split(source->count_, ord_bitset, left_point_writer, right_point_writer, dim == dim_to_clear);
                StorageAssert(right_count == next_right_count, ("wrong number of points in split: expected=" + std::to_string(right_count) + " but actual=" + std::to_string(next_right_count)).c_str());

                left_slices[dim] = std::make_shared<PathSlice>(left_point_writer, 0, left_count);
                right_slices[dim] = std::make_shared<PathSlice>(right_point_writer, 0, right_count);
                left_point_writer->Close();
                right_point_writer->Close();
            }
        }

        parent_splits[split_dim]++;
        Build(2 * nodeID, leaf_node_offset, left_slices,
              ord_bitset, out,
              min_packed_value, max_split_packed_value, parent_splits,
              split_packed_values, leaf_block_fps, to_close_heroically);
        for (int32_t dim = 0; dim < num_index_dims_; dim++) {
            if (dim != split_dim && slices[dim] != nullptr) {
                left_slices[dim]->writer_->Destroy();
            }
        }

        Build(2 * nodeID + 1, leaf_node_offset, right_slices,
              ord_bitset, out,
              min_split_packed_value, max_packed_value, parent_splits,
              split_packed_values, leaf_block_fps, to_close_heroically);
        for (int32_t dim = 0; dim < num_index_dims_; dim++) {
            if (dim != split_dim && slices[dim] != nullptr) {
                right_slices[dim]->writer_->Destroy();
            }
        }
        parent_splits[split_dim]--;
        if (delete_split_dim) {
            slices[split_dim]->writer_->Destroy();
        }
    }
}

}