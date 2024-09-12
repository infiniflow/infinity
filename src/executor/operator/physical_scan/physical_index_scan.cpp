// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <algorithm>
#include <bit>
#include <vector>

module physical_index_scan;

import query_context;
import operator_state;
import default_values;
import buffer_handle;
import infinity_exception;
import logger;
import third_party;
import txn;
import data_block;
import secondary_index_scan_execute_expression;
import logical_type;
import table_index_entry;
import segment_index_entry;
import chunk_index_entry;
import secondary_index_in_mem;
import segment_entry;
import fast_rough_filter;
import roaring_bitmap;
import filter_value_type_classification;
import physical_scan_base;

namespace infinity {

PhysicalIndexScan::PhysicalIndexScan(const u64 id,
                                     SharedPtr<BaseTableRef> base_table_ref,
                                     SharedPtr<BaseExpression> index_filter_qualified,
                                     HashMap<ColumnID, TableIndexEntry *> &&column_index_map,
                                     Vector<FilterExecuteElem> &&filter_execute_command,
                                     UniquePtr<FastRoughFilterEvaluator> &&fast_rough_filter_evaluator,
                                     SharedPtr<Vector<LoadMeta>> load_metas,
                                     SharedPtr<Vector<String>> output_names,
                                     SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                     const bool add_row_id)
    : PhysicalScanBase(id, PhysicalOperatorType::kIndexScan, nullptr, nullptr, std::move(base_table_ref), std::move(load_metas)),
      index_filter_qualified_(std::move(index_filter_qualified)), column_index_map_(std::move(column_index_map)),
      filter_execute_command_(std::move(filter_execute_command)), fast_rough_filter_evaluator_(std::move(fast_rough_filter_evaluator)),
      output_names_(std::move(output_names)), output_types_(std::move(output_types)), add_row_id_(add_row_id) {
    // check last output column
    if (output_types_->size() != output_names_->size()) {
        UnrecoverableError("PhysicalIndexScan::PhysicalIndexScan(): output_names_ mismatch with output_types_");
    }
    if (!(add_row_id_ ? (output_types_->size() == 1 && output_types_->back()->type() == LogicalType::kRowID) : (output_types_->size() == 0))) {
        UnrecoverableError("PhysicalIndexScan::PhysicalIndexScan(): add_row_id_ mismatch with output_types_");
    }
}

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalIndexScan::PlanBlockEntries(i64) const {
    UnrecoverableError("PhysicalIndexScan::PlanBlockEntries(): should not be called.");
    return {};
}

Vector<UniquePtr<Vector<SegmentID>>> PhysicalIndexScan::PlanSegments(u32 parallel_count) const {
    const u32 total_segment_num = base_table_ref_->block_index_->SegmentCount();
    const u32 segment_num_per_tasklet = total_segment_num / parallel_count;
    const u32 segment_num_remainder = total_segment_num % parallel_count;
    SegmentID next_segment_id = 0;
    Vector<UniquePtr<Vector<SegmentID>>> result;
    result.reserve(parallel_count);
    for (u32 i = 0; i < parallel_count; ++i) {
        auto segment_ids = MakeUnique<Vector<SegmentID>>();
        u32 segment_num = segment_num_per_tasklet + (i < segment_num_remainder ? 1 : 0);
        segment_ids->reserve(segment_num);
        for (u32 j = 0; j < segment_num; ++j) {
            segment_ids->emplace_back(next_segment_id++);
        }
        result.emplace_back(std::move(segment_ids));
    }
    if (next_segment_id != total_segment_num) {
        String error_message = "PhysicalIndexScan::PlanSegments(): segment number error.";
        UnrecoverableError(error_message);
    }
    return result;
}

void PhysicalIndexScan::Init() {
    // check add_row_id_
    if (!add_row_id_) {
        String error_message = "ExecuteInternal(): add_row_id_ should be true.";
        UnrecoverableError(error_message);
    }
}

bool PhysicalIndexScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto index_scan_operator_state = static_cast<IndexScanOperatorState *>(operator_state);
    // solve one segment in one call
    ExecuteInternal(query_context, index_scan_operator_state);
    return true;
}

template <typename ColumnValueType>
struct TrunkReader {
    virtual ~TrunkReader() = default;
    virtual u32 GetResultCnt(const FilterIntervalRangeT<ColumnValueType> &interval_range) = 0;
    virtual void OutPut(Bitmask &selected_rows) = 0;
};

template <typename ColumnValueType>
struct TrunkReaderT final : public TrunkReader<ColumnValueType> {
    using KeyType = ConvertToOrderedType<ColumnValueType>;
    static constexpr u32 data_pair_size = sizeof(KeyType) + sizeof(u32);
    const u32 segment_row_count_;
    SharedPtr<ChunkIndexEntry> chunk_index_entry_;
    u32 begin_pos_ = 0;
    u32 end_pos_ = 0;
    TrunkReaderT(const u32 segment_row_count, const SharedPtr<ChunkIndexEntry> &chunk_index_entry)
        : segment_row_count_(segment_row_count), chunk_index_entry_(chunk_index_entry) {}
    u32 GetResultCnt(const FilterIntervalRangeT<ColumnValueType> &interval_range) override {
        static_assert(std::is_same_v<KeyType, typename FilterIntervalRangeT<ColumnValueType>::T>);
        BufferHandle index_handle_head = chunk_index_entry_->GetIndex();
        auto index = static_cast<const SecondaryIndexData *>(index_handle_head.GetData());
        u32 index_data_num = index->GetChunkRowCount();
        auto [begin_val, end_val] = interval_range.GetRange();
        // 1. search PGM and get approximate search range
        // result:
        //    1. size_t pos_;         ///< The approximate position of the key.
        //    2. size_t lower_bound_; ///< The lower bound of the range.
        //    3. size_t upper_bound_; ///< The upper bound of the range.
        // NOTICE: PGM return a range [lower_bound_, upper_bound_) which must include **one** key when the key exists
        // NOTICE: but the range may not include the complete [start, end] range
        auto [begin_approx_pos, begin_lower, begin_upper] = index->SearchPGM(&begin_val);
        auto [end_approx_pos, end_lower, end_upper] = index->SearchPGM(&end_val);
        u32 begin_pos = begin_lower;
        u32 end_pos = std::min<u32>(end_upper, index_data_num - 1);
        if (end_pos < begin_pos) {
            return 0;
        }
        const auto column_data_type = chunk_index_entry_->segment_index_entry_->table_index_entry()->column_def()->type();
        const auto index_part_num = chunk_index_entry_->GetPartNum();
        // 2. find the exact range
        // 2.1 find the exact begin_pos which is the first position that index_key >= begin_val
        u32 begin_part_id = begin_pos / 8192;
        u32 begin_part_offset = begin_pos % 8192;
        auto index_handle_b = chunk_index_entry_->GetIndexPartAt(begin_part_id);
        auto index_data_b = index_handle_b.GetData();
        auto index_key_b_ptr = [&index_data_b](u32 i) -> KeyType {
            KeyType key = {};
            std::memcpy(&key, static_cast<const char *>(index_data_b) + i * data_pair_size, sizeof(KeyType));
            return key;
        };
        auto begin_part_size = chunk_index_entry_->GetPartRowCount(begin_part_id);
        if (index_key_b_ptr(begin_part_offset) < begin_val) {
            // search forward
            while (index_key_b_ptr(begin_part_offset) < begin_val) {
                if (++begin_part_offset == begin_part_size) {
                    if (++begin_part_id >= index_part_num) {
                        // nothing found
                        return 0;
                    }
                    index_handle_b = chunk_index_entry_->GetIndexPartAt(begin_part_id);
                    index_data_b = index_handle_b.GetData();
                    begin_part_size = chunk_index_entry_->GetPartRowCount(begin_part_id);
                    begin_part_offset = 0;
                }
            }
        } else {
            // search backward
            auto test_begin_part_id = begin_part_id;
            auto test_begin_part_offset = begin_part_offset;
            while (index_key_b_ptr(test_begin_part_offset) >= begin_val) {
                // keep valid begin_pos
                begin_part_id = test_begin_part_id;
                begin_part_offset = test_begin_part_offset;
                if (test_begin_part_offset-- == 0) {
                    if (test_begin_part_id-- == 0) {
                        // left bound is the leftmost
                        break;
                    }
                    index_handle_b = chunk_index_entry_->GetIndexPartAt(test_begin_part_id);
                    index_data_b = index_handle_b.GetData();
                    begin_part_size = chunk_index_entry_->GetPartRowCount(test_begin_part_id);
                    test_begin_part_offset = begin_part_size - 1;
                }
            }
            // recover valid pointers
            index_handle_b = chunk_index_entry_->GetIndexPartAt(begin_part_id);
            index_data_b = index_handle_b.GetData();
            begin_part_size = chunk_index_entry_->GetPartRowCount(begin_part_id);
        }
        // update begin_pos
        begin_pos = begin_part_id * 8192 + begin_part_offset;
        // 2.2 find the exact end_pos which is the first position that index_key > end_val (or the position past the end)
        u32 end_part_id = end_pos / 8192;
        u32 end_part_offset = end_pos % 8192;
        auto index_handle_e = chunk_index_entry_->GetIndexPartAt(end_part_id);
        auto index_data_e = index_handle_e.GetData();
        auto index_key_e_ptr = [&index_data_e](u32 i) -> KeyType {
            KeyType key = {};
            std::memcpy(&key, static_cast<const char *>(index_data_e) + i * data_pair_size, sizeof(KeyType));
            return key;
        };
        auto end_part_size = chunk_index_entry_->GetPartRowCount(end_part_id);
        if (index_key_e_ptr(end_part_offset) <= end_val) {
            // search forward
            while (index_key_e_ptr(end_part_offset) <= end_val) {
                if (++end_part_offset == end_part_size) {
                    if (++end_part_id >= index_part_num) {
                        // right bound is the rightmost
                        // recover end_part_id and keep end_part_offset
                        // they will be used to calculate end_pos
                        --end_part_id;
                        break;
                    }
                    index_handle_e = chunk_index_entry_->GetIndexPartAt(end_part_id);
                    index_data_e = index_handle_e.GetData();
                    end_part_size = chunk_index_entry_->GetPartRowCount(end_part_id);
                    end_part_offset = 0;
                }
            }
        } else {
            // search backward
            auto test_end_part_id = end_part_id;
            auto test_end_part_offset = end_part_offset;
            while (index_key_e_ptr(test_end_part_offset) > end_val) {
                end_part_id = test_end_part_id;
                end_part_offset = test_end_part_offset;
                if (test_end_part_offset-- == 0) {
                    if (test_end_part_id-- == 0) {
                        // nothing found
                        return 0;
                    }
                    index_handle_e = chunk_index_entry_->GetIndexPartAt(test_end_part_id);
                    index_data_e = index_handle_e.GetData();
                    // no need to update end_part_size
                    test_end_part_offset = chunk_index_entry_->GetPartRowCount(test_end_part_id) - 1;
                }
            }
            // does not need to recover valid values like index_handle_e, index_data_e, index_key_e_ptr, end_part_size
        }
        // update end_pos
        end_pos = end_part_id * 8192 + end_part_offset;
        // 3. now we know result size
        if (end_pos <= begin_pos) {
            // nothing found
            return 0;
        }
        // have result
        begin_pos_ = begin_pos;
        end_pos_ = end_pos;
        const u32 result_size = end_pos - begin_pos;
        return result_size;
    }
    void OutPut(Bitmask &selected_rows) override {
        const u32 begin_pos = begin_pos_;
        const u32 end_pos = end_pos_;
        const u32 result_size = end_pos - begin_pos;
        u32 begin_part_id = begin_pos / 8192;
        u32 begin_part_offset = begin_pos % 8192;
        auto index_handle_b = chunk_index_entry_->GetIndexPartAt(begin_part_id);
        auto index_data_b = index_handle_b.GetData();
        auto index_offset_b_ptr = [&index_data_b](const u32 i) -> u32 {
            u32 result = 0;
            std::memcpy(&result, static_cast<const char *>(index_data_b) + i * data_pair_size + sizeof(KeyType), sizeof(u32));
            return result;
        };
        auto begin_part_size = chunk_index_entry_->GetPartRowCount(begin_part_id);
        // output result
        for (u32 i = 0; i < result_size; ++i) {
            if (begin_part_offset == begin_part_size) {
                index_handle_b = chunk_index_entry_->GetIndexPartAt(++begin_part_id);
                index_data_b = index_handle_b.GetData();
                begin_part_size = chunk_index_entry_->GetPartRowCount(begin_part_id);
                begin_part_offset = 0;
            }
            selected_rows.SetTrue(index_offset_b_ptr(begin_part_offset));
            ++begin_part_offset;
        }
    }
};

template <typename ColumnValueType>
struct TrunkReaderM final : public TrunkReader<ColumnValueType> {
    using KeyType = ConvertToOrderedType<ColumnValueType>;
    const u32 segment_row_count_;
    SharedPtr<SecondaryIndexInMem> memory_secondary_index_;
    Pair<u32, Bitmask> result_cache_;
    TrunkReaderM(const u32 segment_row_count, const SharedPtr<SecondaryIndexInMem> &memory_secondary_index)
        : segment_row_count_(segment_row_count), memory_secondary_index_(memory_secondary_index) {}
    u32 GetResultCnt(const FilterIntervalRangeT<ColumnValueType> &interval_range) override {
        auto [begin_val, end_val] = interval_range.GetRange();
        Tuple<u32, KeyType, KeyType> arg_tuple = {segment_row_count_, begin_val, end_val};
        result_cache_ = memory_secondary_index_->RangeQuery(&arg_tuple);
        return result_cache_.first;
    }
    void OutPut(Bitmask &selected_rows) override { selected_rows.MergeOr(result_cache_.second); }
};

struct FilterResult {
    Bitmask selected_rows_; // result

    explicit FilterResult(const u32 segment_row_count) : selected_rows_(segment_row_count) {}

    // count of rows in segment, include deleted rows
    [[nodiscard]] inline u32 SegmentRowCount() const { return selected_rows_.count(); }

    [[nodiscard]] inline u32 SelectedNum() const { return selected_rows_.CountTrue(); }

    inline void MergeOr(const FilterResult &other) {
        // merge selected_rows_
        selected_rows_.MergeOr(other.selected_rows_);
    }

    inline void MergeAnd(const FilterResult &other) {
        // merge selected_rows_
        selected_rows_.MergeAnd(other.selected_rows_);
    }

    inline void SetEmptyResult() { selected_rows_.SetAllFalse(); }

    template <typename ColumnValueType>
    inline void ExecuteSingleRangeT(const FilterIntervalRangeT<ColumnValueType> &interval_range, SegmentIndexEntry &index_entry, Txn *txn) {
        Vector<UniquePtr<TrunkReader<ColumnValueType>>> trunk_readers;
        Tuple<Vector<SharedPtr<ChunkIndexEntry>>, SharedPtr<SecondaryIndexInMem>> chunks_snapshot = index_entry.GetSecondaryIndexSnapshot();
        const u32 segment_row_count = SegmentRowCount();
        auto &[chunk_index_entries, memory_secondary_index] = chunks_snapshot;
        for (const auto &chunk_index_entry : chunk_index_entries) {
            if (chunk_index_entry->CheckVisible(txn)) {
                trunk_readers.emplace_back(MakeUnique<TrunkReaderT<ColumnValueType>>(segment_row_count, chunk_index_entry));
            }
        }
        if (memory_secondary_index) {
            trunk_readers.emplace_back(MakeUnique<TrunkReaderM<ColumnValueType>>(segment_row_count, memory_secondary_index));
        }
        // output result
        SetEmptyResult();
        for (auto &trunk_reader : trunk_readers) {
            // real calculation is done here
            if (const auto part_result_size = trunk_reader->GetResultCnt(interval_range); part_result_size) {
                trunk_reader->OutPut(selected_rows_);
            }
        }
        selected_rows_.RunOptimize();
    }

    inline void ExecuteSingleRange(const HashMap<ColumnID, TableIndexEntry *> &column_index_map,
                                   const FilterExecuteSingleRange &single_range,
                                   SegmentID segment_id,
                                   Txn *txn) {
        // step 1. check if range is empty
        if (single_range.IsEmpty()) {
            return SetEmptyResult();
        }
        // step 2. get ColumnID and prepare SegmentIndexEntry
        ColumnID column_id = single_range.GetColumnID();
        auto const &index_by_segment = column_index_map.at(column_id)->index_by_segment();
        SegmentIndexEntry &index_entry = *(index_by_segment.at(segment_id));
        // step 3. search index
        auto &interval_range_variant = single_range.GetIntervalRange();
        std::visit(Overload{[&]<typename ColumnValueType>(const FilterIntervalRangeT<ColumnValueType> &interval_range) {
                                ExecuteSingleRangeT(interval_range, index_entry, txn);
                            },
                            [](const std::monostate &empty) {
                                UnrecoverableError("FilterResult::ExecuteSingleRange(): class member interval_range_ not initialized!");
                            }},
                   interval_range_variant);
    }

    inline void Output(Vector<UniquePtr<DataBlock>> &output_data_blocks, SegmentID segment_id, const DeleteFilter &delete_filter) const {
        const u32 block_capacity = DEFAULT_BLOCK_CAPACITY;
        const u32 selected_row_num = SelectedNum(); // before delete filter
        // check if output_data_blocks is empty
        if (!output_data_blocks.empty()) {
            UnrecoverableError("FilterResult::Output(): output data block array should be empty.");
        }
        Vector<SharedPtr<DataType>> output_types;
        output_types.emplace_back(MakeShared<DataType>(LogicalType::kRowID));
        // 1. prepare first output_data_block
        auto append_data_block = [&]() {
            auto data_block = DataBlock::MakeUniquePtr();
            // u32 write_size = std::min(block_capacity, selected_row_num - row_idx);
            // TODO: error: if Init with write_size != pow of 2, error will occur in Bitmask::Initialize()
            data_block->Init(output_types);
            output_data_blocks.emplace_back(std::move(data_block));
        };
        append_data_block();
        // 2. output
        // delete_filter: return false if the row is deleted
        u32 output_rows = 0;
        u32 invalid_rows = 0;
        {
            u32 output_block_row_id = 0;
            DataBlock *output_block_ptr = output_data_blocks.back().get();
            const u32 segment_row_count = SegmentRowCount();
            selected_rows_.RoaringBitmapApplyFunc([&](const u32 segment_offset) -> bool {
                if (segment_offset >= segment_row_count) [[unlikely]] {
                    UnrecoverableError("FilterResult::Output(): segment_offset out of range.");
                }
                if (!delete_filter(segment_offset)) {
                    // deleted
                    ++invalid_rows;
                    return true;
                }
                if (output_block_row_id == block_capacity) {
                    output_block_ptr->Finalize();
                    append_data_block();
                    output_block_ptr = output_data_blocks.back().get();
                    output_block_row_id = 0;
                }
                RowID row_id(segment_id, segment_offset);
                output_block_ptr->AppendValueByPtr(0, reinterpret_cast<ptr_t>(&row_id));
                ++output_block_row_id;
                ++output_rows;
                return true;
            });
            output_block_ptr->Finalize();
            if (output_rows + invalid_rows != selected_row_num) {
                UnrecoverableError("FilterResult::Output(): output row num error.");
            }
        }
        LOG_INFO(fmt::format("FilterResult::Output(): output rows: {}, invalid candidate rows: {}", output_rows, invalid_rows));
    }
};

FilterResult SolveSecondaryIndexFilterInner(const Vector<FilterExecuteElem> &filter_execute_command,
                                            const HashMap<ColumnID, TableIndexEntry *> &column_index_map,
                                            const SegmentID segment_id,
                                            const u32 segment_row_count,
                                            Txn *txn) {
    Vector<FilterResult> result_stack;
    // execute filter_execute_command_ (Reverse Polish notation)
    for (auto const &elem : filter_execute_command) {
        std::visit(Overload{[&](FilterExecuteCombineType combine_type) {
                                switch (combine_type) {
                                    case FilterExecuteCombineType::kOr: {
                                        if (auto v_size = result_stack.size(); v_size >= 2) {
                                            auto &right = result_stack[v_size - 1];
                                            result_stack[v_size - 2].MergeOr(right);
                                            result_stack.pop_back();
                                        } else {
                                            UnrecoverableError("SolveSecondaryIndexFilterInner(): filter result stack error.");
                                        }
                                        break;
                                    }
                                    case FilterExecuteCombineType::kAnd: {
                                        if (auto v_size = result_stack.size(); v_size >= 2) {
                                            auto &right = result_stack[v_size - 1];
                                            result_stack[v_size - 2].MergeAnd(right);
                                            result_stack.pop_back();
                                        } else {
                                            UnrecoverableError("SolveSecondaryIndexFilterInner(): filter result stack error.");
                                        }
                                        break;
                                    }
                                }
                            },
                            [&](const FilterExecuteSingleRange &single_range) {
                                result_stack.emplace_back(segment_row_count);
                                result_stack.back().ExecuteSingleRange(column_index_map, single_range, segment_id, txn);
                            }},
                   elem);
    }
    // check if result is valid
    if (result_stack.size() != 1) {
        UnrecoverableError("SolveSecondaryIndexFilterInner(): filter result stack error.");
    }
    return std::move(result_stack[0]);
}

Bitmask SolveSecondaryIndexFilter(const Vector<FilterExecuteElem> &filter_execute_command,
                                  const HashMap<ColumnID, TableIndexEntry *> &column_index_map,
                                  const SegmentID segment_id,
                                  const u32 segment_row_count,
                                  Txn *txn) {
    if (filter_execute_command.empty()) {
        // return all true
        Bitmask res(segment_row_count);
        return res;
    }
    auto result = SolveSecondaryIndexFilterInner(filter_execute_command, column_index_map, segment_id, segment_row_count, txn);
    return std::move(result.selected_rows_);
}

void PhysicalIndexScan::ExecuteInternal(QueryContext *query_context, IndexScanOperatorState *index_scan_operator_state) const {
    Txn *txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();

    auto &output_data_blocks = index_scan_operator_state->data_block_array_;
    auto &segment_ids = *(index_scan_operator_state->segment_ids_);
    auto &next_idx = index_scan_operator_state->next_idx_;
    if (!output_data_blocks.empty()) {
        UnrecoverableError("Index scan output data block array should be empty");
    }
    // check before execute
    if (next_idx >= segment_ids.size()) {
        // Already finished
        index_scan_operator_state->SetComplete();
        return;
    }
    // get the segment id to solve
    auto segment_id = segment_ids[next_idx];

    SegmentEntry *segment_entry = nullptr;
    SegmentOffset segment_row_count = 0; // count of rows in segment, include deleted rows
    const auto &segment_block_index_ = base_table_ref_->block_index_->segment_block_index_;
    if (auto iter = segment_block_index_.find(segment_id); iter == segment_block_index_.end()) {
        UnrecoverableError(fmt::format("Cannot find SegmentEntry for segment id: {}", segment_id));
    } else {
        segment_entry = iter->second.segment_entry_;
        segment_row_count = iter->second.segment_offset_;
    }
    // check FastRoughFilter
    const auto &fast_rough_filter = *segment_entry->GetFastRoughFilter();
    if (fast_rough_filter_evaluator_ and !fast_rough_filter_evaluator_->Evaluate(begin_ts, fast_rough_filter)) {
        // skip this segment
        LOG_TRACE(fmt::format("IndexScan: job number: {}, segment_ids.size(): {}, skipped after FastRoughFilter", next_idx, segment_ids.size()));
        // output one empty data block
        // some operator expect at least one input block
        Vector<SharedPtr<DataType>> output_types;
        output_types.emplace_back(MakeShared<DataType>(LogicalType::kRowID));
        auto data_block = DataBlock::MakeUniquePtr();
        data_block->Init(output_types);
        output_data_blocks.emplace_back(std::move(data_block));
        // update next_idx
        // check if jobs are all done
        if (++next_idx >= segment_ids.size()) {
            // Finished
            index_scan_operator_state->SetComplete();
        }
        return;
    }
    LOG_TRACE(fmt::format("IndexScan: job number: {}, segment_ids.size(): {}, not skipped after FastRoughFilter", next_idx, segment_ids.size()));

    // prepare filter for deleted rows
    DeleteFilter delete_filter(segment_entry, begin_ts, segment_row_count);
    // output
    const auto result = SolveSecondaryIndexFilterInner(filter_execute_command_, column_index_map_, segment_id, segment_row_count, txn);
    result.Output(output_data_blocks, segment_id, delete_filter);

    LOG_TRACE(fmt::format("IndexScan: job number: {}, segment_ids.size(): {}, finished", next_idx, segment_ids.size()));
    // update next_idx
    // check if jobs are all done
    if (++next_idx >= segment_ids.size()) {
        // Finished
        index_scan_operator_state->SetComplete();
    }
}

} // namespace infinity
