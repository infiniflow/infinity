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
import :stl;
import :physical_operator_type;
import :default_values;
import :operator_state;
import :column_vector;
import :query_context;

import :base_table_ref;
import :third_party;
import :infinity_exception;
import logical_type;
import internal_types;
import :result_cache_manager;
import :logger;
import :data_block;
import :cached_match;
import :buffer_manager;
import :block_index;

import :block_meta;
import :column_meta;
import :new_catalog;
import :status;
import data_type;

namespace infinity {

SizeT PhysicalOperator::TaskletCount() { return 1; }

String PhysicalOperator::GetName() const { return PhysicalOperatorToString(operator_type_); }

void PhysicalOperator::InputLoad(QueryContext *query_context, OperatorState *operator_state, HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) {
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
    for (SizeT i = 0; i < operator_state->prev_op_state_->data_block_array_.size(); ++i) {
        auto input_block = operator_state->prev_op_state_->data_block_array_[i].get();
        SizeT load_column_count = load_metas_->size();

        u16 row_count = input_block->row_count();
        SizeT capacity = input_block->capacity();

        // Filling ColumnVector
        for (SizeT j = 0; j < load_column_count; ++j) {
            SharedPtr<ColumnVector> column_vector = ColumnVector::Make(load_metas[j].type_);
            auto column_vector_type =
                (load_metas[j].type_->type() == LogicalType::kBoolean) ? ColumnVectorType::kCompactBit : ColumnVectorType::kFlat;
            column_vector->Initialize(column_vector_type, capacity);
            column_vector->Finalize(row_count);
            input_block->InsertVector(column_vector, load_metas[j].index_);
        }

        auto row_column_id = input_block->column_count() - 1;

        for (SizeT j = 0; j < row_count; ++j) {
            // If late materialization needs to be optional, then this needs to be modified
            RowID row_id = input_block->GetValue(row_column_id, j).value_.row;
            u32 segment_id = row_id.segment_id_;
            u32 segment_offset = row_id.segment_offset_;
            u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
            u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
            for (SizeT k = 0; k < load_column_count; ++k) {
                output_to_data_block_helper
                    .AddOutputJobInfo(segment_id, block_id, load_metas[k].binding_.column_idx, block_offset, i, load_metas[k].index_, j);
            }
        }
    }
    output_to_data_block_helper.OutputToDataBlock(query_context->storage()->buffer_manager(),
                                                  table_ref->block_index_.get(),
                                                  operator_state->prev_op_state_->data_block_array_);
}

SharedPtr<Vector<String>> PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(const PhysicalOperator &op) {
    auto prev_output_names = op.left()->GetOutputNames();
    auto output_names = MakeShared<Vector<String>>(*prev_output_names);
    if (auto &ptr = op.load_metas(); ptr) {
        for (auto &load_meta : *ptr) {
            output_names->insert(output_names->begin() + load_meta.index_, load_meta.column_name_);
        }
    }
    return output_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(const PhysicalOperator &op) {
    auto prev_output_types = op.left()->GetOutputTypes();
    auto output_types = MakeShared<Vector<SharedPtr<DataType>>>(*prev_output_types);
    if (auto &ptr = op.load_metas(); ptr) {
        for (auto &load_meta : *ptr) {
            output_types->insert(output_types->begin() + load_meta.index_, load_meta.type_);
        }
    }
    return output_types;
}

void OutputToDataBlockHelper::OutputToDataBlock(BufferManager *buffer_mgr,
                                                const BlockIndex *block_index,
                                                const Vector<UniquePtr<DataBlock>> &output_data_blocks) {
    std::sort(output_job_infos.begin(), output_job_infos.end());

    Status status;

    auto cache_segment_id = std::numeric_limits<SegmentID>::max();
    auto cache_block_id = std::numeric_limits<BlockID>::max();
    // BlockEntry *cache_block_entry = nullptr;
    BlockMeta *cached_block_meta = nullptr;
    SizeT cached_block_row_cnt = 0;
    auto cache_column_id = std::numeric_limits<ColumnID>::max();
    ColumnVector cache_column_vector;
    Vector<Pair<SegmentID, BlockID>> segment_block_ids_without_blockmeta;
    for (const auto [segment_id, block_id, column_id, block_offset, output_block_id, output_column_id, output_row_id] : output_job_infos) {
        if (segment_id != cache_segment_id || block_id != cache_block_id) {
            cache_segment_id = segment_id;
            cache_block_id = block_id;
            cached_block_meta = block_index->GetBlockMeta(segment_id, block_id);
            if (cached_block_meta != nullptr) {
                std::tie(cached_block_row_cnt, status) = cached_block_meta->GetRowCnt1();
                if (!status.ok()) {
                    RecoverableError(status);
                }
                cache_column_id = std::numeric_limits<ColumnID>::max();
            } else {
                segment_block_ids_without_blockmeta.emplace_back(segment_id, block_id);
                continue;
            }
        }
        if (column_id != cache_column_id) {
            // LOG_TRACE(fmt::format("Get column vector from segment_id: {}, block_id: {}, column_id: {}", segment_id, block_id, column_id));
            ColumnMeta column_meta(column_id, *cached_block_meta);
            NewCatalog::GetColumnVector(column_meta,
                                        column_meta.get_column_def(),
                                        cached_block_row_cnt,
                                        ColumnVectorMode::kReadOnly,
                                        cache_column_vector);

            cache_column_id = column_id;
        }
        auto val_for_update = cache_column_vector.GetValueByIndex(block_offset);
        output_data_blocks[output_block_id]->column_vectors[output_column_id]->SetValueByIndex(output_row_id, val_for_update);
    }
    output_job_infos.clear();

    if (segment_block_ids_without_blockmeta.size() > 0) {
        OStringStream oss;
        for (auto &segment_block_id : segment_block_ids_without_blockmeta) {
            oss << fmt::format("({}, {}) ", segment_block_id.first, segment_block_id.second);
        }
        LOG_WARN(fmt::format("BlockMeta not found. (segment_id, block_id): [ {}]", oss.str()));
    }
}

} // namespace infinity
