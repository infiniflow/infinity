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

#include <string>
#include <vector>

module infinity_core:physical_operator.impl;

import :physical_operator;
import :physical_operator_type;
import :default_values;
import :operator_state;
import :column_vector;
import :query_context;
import :base_table_ref;
import :infinity_exception;
import :result_cache_manager;
import :logger;
import :data_block;
import :cached_match;

import :block_index;
import :block_meta;
import :column_meta;
import :new_catalog;
import :status;

import std;
import third_party;

import data_type;
import logical_type;
import internal_types;

namespace infinity {

size_t PhysicalOperator::TaskletCount() { return 1; }

std::string PhysicalOperator::GetName() const { return PhysicalOperatorToString(operator_type_); }

void PhysicalOperator::InputLoad(QueryContext *query_context,
                                 OperatorState *operator_state,
                                 std::unordered_map<size_t, std::shared_ptr<BaseTableRef>> &table_refs) {
    if (load_metas_.get() == nullptr || load_metas_->empty()) {
        return;
    }
    //    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();

    auto load_metas = *load_metas_.get();
    // FIXME: After columnar reading is supported, use a different table_ref for each LoadMetas
    const auto table_refs_it = table_refs.find(load_metas[0].binding_.table_idx);
    if (table_refs_it == table_refs.end()) {
        UnrecoverableError("TableRef not found");
    }
    const auto *table_ref = table_refs_it->second.get();

    OutputToDataBlockHelper output_to_data_block_helper;
    for (size_t i = 0; i < operator_state->prev_op_state_->data_block_array_.size(); ++i) {
        auto input_block = operator_state->prev_op_state_->data_block_array_[i].get();
        size_t load_column_count = load_metas_->size();

        u16 row_count = input_block->row_count();
        size_t capacity = input_block->capacity();

        // Filling ColumnVector
        for (size_t j = 0; j < load_column_count; ++j) {
            std::shared_ptr<ColumnVector> column_vector = ColumnVector::Make(load_metas[j].type_);
            auto column_vector_type =
                (load_metas[j].type_->type() == LogicalType::kBoolean) ? ColumnVectorType::kCompactBit : ColumnVectorType::kFlat;
            column_vector->Initialize(column_vector_type, capacity);
            column_vector->Finalize(row_count);
            input_block->InsertVector(column_vector, load_metas[j].index_);
        }

        auto row_column_id = input_block->column_count() - 1;

        for (size_t j = 0; j < row_count; ++j) {
            // If late materialization needs to be optional, then this needs to be modified
            RowID row_id = input_block->GetValue(row_column_id, j).value_.row;
            u32 segment_id = row_id.segment_id_;
            u32 segment_offset = row_id.segment_offset_;
            u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
            u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
            for (size_t k = 0; k < load_column_count; ++k) {
                output_to_data_block_helper
                    .AddOutputJobInfo(segment_id, block_id, load_metas[k].binding_.column_idx, block_offset, i, load_metas[k].index_, j);
            }
        }
    }
    output_to_data_block_helper.OutputToDataBlock(query_context->storage()->fileworker_manager(),
                                                  table_ref->block_index_.get(),
                                                  operator_state->prev_op_state_->data_block_array_);
}

std::shared_ptr<std::vector<std::string>> PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(const PhysicalOperator &op) {
    auto prev_output_names = op.left()->GetOutputNames();
    auto output_names = std::make_shared<std::vector<std::string>>(*prev_output_names);
    if (auto &ptr = op.load_metas(); ptr) {
        for (auto &load_meta : *ptr) {
            output_names->insert(output_names->begin() + load_meta.index_, load_meta.column_name_);
        }
    }
    return output_names;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(const PhysicalOperator &op) {
    auto prev_output_types = op.left()->GetOutputTypes();
    auto output_types = std::make_shared<std::vector<std::shared_ptr<DataType>>>(*prev_output_types);
    if (auto &ptr = op.load_metas(); ptr) {
        for (auto &load_meta : *ptr) {
            output_types->insert(output_types->begin() + load_meta.index_, load_meta.type_);
        }
    }
    return output_types;
}

// Helper function to copy a single value from source to target column vector
inline static void CopySingleValue(const ColumnVector &src_col, size_t src_offset, ColumnVector &dst_col, size_t dst_offset) {
    auto val = src_col.GetValueByIndex(src_offset);
    dst_col.SetValueByIndex(dst_offset, val);
}

// Helper struct for hashing std::pair<SegmentID, BlockID>
struct BlockKeyHash {
    std::size_t operator()(const std::pair<SegmentID, BlockID> &key) const noexcept {
        return std::hash<SegmentID>{}(key.first) ^ (std::hash<BlockID>{}(key.second) << 1);
    }
};

void OutputToDataBlockHelper::OutputToDataBlock(FileWorkerManager *fileworker_mgr,
                                                const BlockIndex *block_index,
                                                const std::vector<std::unique_ptr<DataBlock>> &output_data_blocks) {
    std::ranges::sort(output_job_infos);

    Status status;
    std::vector<std::pair<SegmentID, BlockID>> segment_block_ids_without_blockmeta;

    // ===== OPTIMIZATION: Batch processing by (segment_id, block_id) =====
    // Group jobs by (segment_id, block_id) to load all columns of a block at once
    // This reduces the number of GetColumnVector calls from O(N) to O(unique_blocks * unique_columns)
    using BlockKey = std::pair<SegmentID, BlockID>;
    std::unordered_map<BlockKey, std::vector<OutputJobInfo>, BlockKeyHash> block_jobs_map;
    for (const auto &job : output_job_infos) {
        block_jobs_map[{job.segment_id_, job.block_id_}].push_back(job);
    }

    // Process each block
    for (auto &block_entry : block_jobs_map) {
        auto &block_key = block_entry.first;
        auto &jobs = block_entry.second;
        auto segment_id = block_key.first;
        auto block_id = block_key.second;

        // Get BlockMeta
        BlockMeta *block_meta = block_index->GetBlockMeta(segment_id, block_id);
        if (block_meta == nullptr) {
            segment_block_ids_without_blockmeta.emplace_back(segment_id, block_id);
            continue;
        }

        size_t row_cnt;
        std::tie(row_cnt, status) = block_meta->GetRowCnt1();
        if (!status.ok()) {
            RecoverableError(status);
            continue;
        }

        // ===== OPTIMIZATION: Group by column_id to batch-load columns =====
        // All columns for this block are loaded once, then reused for all jobs
        std::unordered_map<ColumnID, std::vector<size_t>> column_job_indices;
        for (size_t i = 0; i < jobs.size(); ++i) {
            column_job_indices[jobs[i].column_id_].push_back(i);
        }

        // Batch load all required columns for this block
        std::unordered_map<ColumnID, ColumnVector> column_vectors;

        for (const auto &[column_id, job_indices] : column_job_indices) {
            ColumnMeta column_meta(column_id, *block_meta);
            auto column_def = column_meta.get_column_def();

            ColumnVector col_vector;
            NewCatalog::GetColumnVector(column_meta, column_def, row_cnt, ColumnVectorMode::kReadOnly, col_vector);

            column_vectors[column_id] = std::move(col_vector);
        }

        // ===== OPTIMIZATION: Batch copy values =====
        // Process all jobs for each column together
        for (const auto &[column_id, col_vector] : column_vectors) {
            const auto &job_indices = column_job_indices[column_id];

            for (size_t idx : job_indices) {
                const auto &job = jobs[idx];
                auto *dst_col = output_data_blocks[job.output_block_id_]->column_vectors_[job.output_column_id_].get();
                CopySingleValue(col_vector, job.block_offset_, *dst_col, job.output_row_id_);
            }
        }
    }

    output_job_infos.clear();

    if (segment_block_ids_without_blockmeta.size() > 0) {
        std::ostringstream oss;
        for (auto &segment_block_id : segment_block_ids_without_blockmeta) {
            oss << fmt::format("({}, {}) ", segment_block_id.first, segment_block_id.second);
        }
        LOG_WARN(fmt::format("BlockMeta not found. (segment_id, block_id): [ {}]", oss.str()));
    }
}

} // namespace infinity
