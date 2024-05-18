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

module physical_limit;

import stl;
import txn;
import base_expression;
import default_values;
import load_meta;
import query_context;
import table_def;
import data_table;
import default_values;

import physical_operator_type;
import operator_state;
import data_block;
import status;
import infinity_exception;
import expression_type;
import value_expression;
import logger;

namespace infinity {

SizeT AtomicCounter::Offset(SizeT row_count) {
    auto success = false;
    SizeT result = 0;

    while (!success) {
        i64 current_offset = offset_;
        if (current_offset <= 0) {
            return 0;
        }
        i64 last_offset = current_offset - row_count;

        if (last_offset > 0) {
            success = offset_.compare_exchange_strong(current_offset, last_offset);
            result = row_count;
        } else {
            success = offset_.compare_exchange_strong(current_offset, 0);
            result = current_offset;
        }
    }

    return result;
}

SizeT AtomicCounter::Limit(SizeT row_count) {
    auto success = false;
    SizeT result = 0;

    while (!success) {
        i64 current_limit = limit_;
        if (current_limit <= 0) {
            return 0;
        }
        i64 last_limit = current_limit - row_count;

        if (last_limit > 0) {
            success = limit_.compare_exchange_strong(current_limit, last_limit);
            result = row_count;
        } else {
            success = limit_.compare_exchange_strong(current_limit, 0);
            result = current_limit;
        }
    }

    return result;
}

bool AtomicCounter::IsLimitOver() {
    if (limit_ < 0) {
        Status status = Status::InvalidParameterValue("Limit", std::to_string(limit_), "larger than 0");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    return limit_ == 0;
}

SizeT UnSyncCounter::Offset(SizeT row_count) {
    SizeT result = 0;

    if (offset_ <= 0) {
        return 0;
    }
    i64 last_offset = offset_ - row_count;

    if (last_offset > 0) {
        result = row_count - 1;
        offset_ = last_offset;
    } else {
        result = offset_;
        offset_ = 0;
    }

    return result;
}

SizeT UnSyncCounter::Limit(SizeT row_count) {
    SizeT result = 0;

    if (limit_ <= 0) {
        return 0;
    }
    i64 last_limit = limit_ - row_count;

    if (last_limit > 0) {
        result = row_count;
        limit_ = last_limit;
    } else {
        result = limit_;
        limit_ = 0;
    }

    return result;
}

bool UnSyncCounter::IsLimitOver() {
    if (limit_ < 0) {
        Status status = Status::InvalidParameterValue("Limit", std::to_string(limit_), "larger than 0");
        LOG_ERROR(status.message());
        RecoverableError(status);
    }
    return limit_ == 0;
}

PhysicalLimit::PhysicalLimit(u64 id,
                             UniquePtr<PhysicalOperator> left,
                             SharedPtr<BaseExpression> limit_expr,
                             SharedPtr<BaseExpression> offset_expr,
                             SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kLimit, std::move(left), nullptr, id, load_metas), limit_expr_(std::move(limit_expr)),
      offset_expr_(std::move(offset_expr)) {
    i64 offset = 0;
    i64 limit = (static_pointer_cast<ValueExpression>(limit_expr_))->GetValue().value_.big_int;

    if (offset_expr_.get() != nullptr) {
        offset = (static_pointer_cast<ValueExpression>(offset_expr_))->GetValue().value_.big_int;
    }

    counter_ = MakeUnique<UnSyncCounter>(offset, limit);
}

void PhysicalLimit::Init() {}

//    offset     limit + offset
//    left       right
// | a | b | c | d | e | f
bool PhysicalLimit::Execute(QueryContext *query_context,
                            const Vector<UniquePtr<DataBlock>> &input_blocks,
                            Vector<UniquePtr<DataBlock>> &output_blocks,
                            LimitCounter *counter) {
    SizeT input_row_count = 0;

    for (SizeT block_id = 0; block_id < input_blocks.size(); block_id++) {
        input_row_count += input_blocks[block_id]->row_count();
    }

    SizeT offset = counter->Offset(input_row_count);
    if (offset == input_row_count) {
        return true;
    }

    SizeT limit = counter->Limit(input_row_count - offset);
    SizeT block_start_idx = 0;

    for (SizeT block_id = 0; block_id < input_blocks.size(); block_id++) {
        if (input_blocks[block_id]->row_count() == 0) {
            continue;
        }
        SizeT row_count = input_blocks[block_id]->row_count();

        if (offset > row_count) {
            offset -= row_count;
        } else {
            block_start_idx = block_id;
            break;
        }
    }

    for (SizeT block_id = block_start_idx; block_id < input_blocks.size(); block_id++) {
        auto &input_block = input_blocks[block_id];
        auto row_count = input_block->row_count();
        if (row_count == 0) {
            continue;
        }
        auto block = DataBlock::MakeUniquePtr();

        block->Init(input_block->types());
        if (limit >= row_count) {
            block->AppendWith(input_block.get(), offset, row_count);
            limit -= row_count;
        } else {
            block->AppendWith(input_block.get(), offset, limit);
            limit = 0;
        }
        block->Finalize();
        output_blocks.push_back(std::move(block));
        offset = 0;

        if (limit == 0) {
            break;
        }
    }

    return true;
}

bool PhysicalLimit::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto result = Execute(query_context, operator_state->prev_op_state_->data_block_array_, operator_state->data_block_array_, counter_.get());

    operator_state->prev_op_state_->data_block_array_.clear();
    if (counter_->IsLimitOver() || operator_state->prev_op_state_->Complete()) {
        operator_state->SetComplete();
    }
    return result;
}

} // namespace infinity
