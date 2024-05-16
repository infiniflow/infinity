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

#include <concepts>
#include <type_traits>

export module binary_operator;

import stl;
import column_vector;
import vector_buffer;

import infinity_exception;
import bitmask;
import bitmask_buffer;
import third_party;
import internal_types;
import status;

namespace infinity {

template <typename LeftType, typename RightType, typename Operator>
    requires std::same_as<LeftType, RightType> // if they are not same, we need to implement a new function
class BooleanResultBinaryOperator {
public:
    static void inline Execute(const SharedPtr<ColumnVector> &left,
                               const SharedPtr<ColumnVector> &right,
                               SharedPtr<ColumnVector> &result,
                               SizeT count,
                               void *state_ptr,
                               bool nullable) {

        auto left_vector_type = left->vector_type();
        auto right_vector_type = right->vector_type();
        auto check_vector_type_valid = [](ColumnVectorType vector_type) {
            // only support kFlat and kConstant
            return vector_type == ColumnVectorType::kFlat || vector_type == ColumnVectorType::kConstant;
        };
        if (!check_vector_type_valid(left_vector_type) || !check_vector_type_valid(right_vector_type)) {
            UnrecoverableError("Invalid input ColumnVectorType. Support only kFlat and kConstant.");
        }
        const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
        const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;
        if (left_vector_type == ColumnVectorType::kConstant && right_vector_type == ColumnVectorType::kConstant) {
            if (!nullable || (left_null->IsAllTrue() && right_null->IsAllTrue())) {
                auto left_ptr = ColumnValueReader<LeftType>(left);
                auto right_ptr = ColumnValueReader<RightType>(right);
                BooleanColumnWriter result_ptr(result);
                Operator::template Execute(left_ptr[0], right_ptr[0], result_ptr[0], result_null.get(), 0, state_ptr);
                result_null->SetAllTrue();
            } else {
                result_null->SetAllFalse();
            }
            result->Finalize(1);
        } else if (left_vector_type == ColumnVectorType::kFlat && right_vector_type == ColumnVectorType::kFlat) {
            if (!nullable || (left_null->IsAllTrue() && right_null->IsAllTrue())) {
                result_null->SetAllTrue();
                auto left_ptr = ColumnValueReader<LeftType>(left);
                auto right_ptr = ColumnValueReader<RightType>(right);
                BooleanColumnWriter result_ptr(result);
                for (SizeT i = 0; i < count; ++i) {
                    Operator::template Execute(left_ptr[i], right_ptr[i], result_ptr[i], result_null.get(), 0, state_ptr);
                }
            } else {
                ResultBooleanExecuteWithNull(left, right, result, count, state_ptr);
            }
            result->Finalize(count);
        } else if (left_vector_type == ColumnVectorType::kConstant && right_vector_type == ColumnVectorType::kFlat) {
            auto left_c = ColumnValueReader<LeftType>(left)[0];
            if (nullable && !(left_null->IsAllTrue())) {
                result_null->SetAllFalse();
            } else if (!nullable || (left_null->IsAllTrue() && right_null->IsAllTrue())) {
                result_null->SetAllTrue();
                auto right_ptr = ColumnValueReader<RightType>(right);
                BooleanColumnWriter result_ptr(result);
                for (SizeT i = 0; i < count; ++i) {
                    Operator::template Execute(left_c, right_ptr[i], result_ptr[i], result_null.get(), 0, state_ptr);
                }
            } else {
                ResultBooleanExecuteWithNull(left_c, right, result, count, state_ptr);
            }
            result->Finalize(count);
        } else if (left_vector_type == ColumnVectorType::kFlat && right_vector_type == ColumnVectorType::kConstant) {
            auto right_c = ColumnValueReader<RightType>(right)[0];
            if (nullable && !(right_null->IsAllTrue())) {
                result_null->SetAllFalse();
            } else if (!nullable || (left_null->IsAllTrue() && right_null->IsAllTrue())) {
                result_null->SetAllTrue();
                auto left_ptr = ColumnValueReader<LeftType>(left);
                BooleanColumnWriter result_ptr(result);
                for (SizeT i = 0; i < count; ++i) {
                    Operator::template Execute(left_ptr[i], right_c, result_ptr[i], result_null.get(), 0, state_ptr);
                }
            } else {
                ResultBooleanExecuteWithNull(left, right_c, result, count, state_ptr);
            }
            result->Finalize(count);
        }
    }

private:
    static inline void ResultBooleanExecuteWithNull(const SharedPtr<ColumnVector> &left,
                                                    const SharedPtr<ColumnVector> &right,
                                                    SharedPtr<ColumnVector> &result,
                                                    SizeT count,
                                                    void *state_ptr) {
        const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
        const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (left_null->IsAllTrue()) {
            result_null->DeepCopy(*right_null);
        } else {
            result_null->DeepCopy(*left_null);
            if (!(right_null->IsAllTrue())) {
                result_null->Merge(*right_null);
            }
        }

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        auto left_ptr = ColumnValueReader<LeftType>(left);
        auto right_ptr = ColumnValueReader<RightType>(right);
        BooleanColumnWriter result_ptr(result);
        static_assert(BitmaskBuffer::UNIT_BITS % 8 == 0, "static_assert: BitmaskBuffer::UNIT_BITS % 8 == 0");
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            end_index = std::min(end_index, count);
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                for (SizeT b = start_index; b < end_index; ++b) {
                    Operator::template Execute(left_ptr[b], right_ptr[b], result_ptr[b], result_null.get(), 0, state_ptr);
                }
                start_index = end_index;
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                start_index = end_index;
            } else {
                for (; start_index < end_index; ++start_index) {
                    if (result_null->IsTrue(start_index)) {
                        // This row isn't null
                        Operator::template Execute(left_ptr[start_index],
                                                   right_ptr[start_index],
                                                   result_ptr[start_index],
                                                   result_null.get(),
                                                   start_index,
                                                   state_ptr);
                    }
                }
            }
        }
    }

    static inline void ResultBooleanExecuteWithNull(auto left_constant,
                                                    const SharedPtr<ColumnVector> &right,
                                                    SharedPtr<ColumnVector> &result,
                                                    SizeT count,
                                                    void *state_ptr) {
        const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        result_null->DeepCopy(*right_null);

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        auto right_ptr = ColumnValueReader<RightType>(right);
        BooleanColumnWriter result_ptr(result);
        static_assert(BitmaskBuffer::UNIT_BITS % 8 == 0, "static_assert: BitmaskBuffer::UNIT_BITS % 8 == 0");
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            end_index = std::min(end_index, count);
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                for (SizeT b = start_index; b < end_index; ++b) {
                    Operator::template Execute(left_constant, right_ptr[b], result_ptr[b], result_null.get(), 0, state_ptr);
                }
                start_index = end_index;
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                start_index = end_index;
            } else {
                for (; start_index < end_index; ++start_index) {
                    if (result_null->IsTrue(start_index)) {
                        // This row isn't null
                        Operator::template Execute(left_constant,
                                                   right_ptr[start_index],
                                                   result_ptr[start_index],
                                                   result_null.get(),
                                                   start_index,
                                                   state_ptr);
                    }
                }
            }
        }
    }

    static inline void ResultBooleanExecuteWithNull(const SharedPtr<ColumnVector> &left,
                                                    auto right_constant,
                                                    SharedPtr<ColumnVector> &result,
                                                    SizeT count,
                                                    void *state_ptr) {
        const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        result_null->DeepCopy(*left_null);

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        auto left_ptr = ColumnValueReader<LeftType>(left);
        BooleanColumnWriter result_ptr(result);
        static_assert(BitmaskBuffer::UNIT_BITS % 8 == 0, "static_assert: BitmaskBuffer::UNIT_BITS % 8 == 0");
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            end_index = std::min(end_index, count);
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                for (SizeT b = start_index; b < end_index; ++b) {
                    Operator::template Execute(left_ptr[b], right_constant, result_ptr[b], result_null.get(), 0, state_ptr);
                }
                start_index = end_index;
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                start_index = end_index;
            } else {
                for (; start_index < end_index; ++start_index) {
                    if (result_null->IsTrue(start_index)) {
                        // This row isn't null
                        Operator::template Execute(left_ptr[start_index],
                                                   right_constant,
                                                   result_ptr[start_index],
                                                   result_null.get(),
                                                   start_index,
                                                   state_ptr);
                    }
                }
            }
        }
    }
};

template <typename Operator>
class BooleanResultBinaryOperator<BooleanT, BooleanT, Operator> {
public:
    static void inline Execute(const SharedPtr<ColumnVector> &left,
                               const SharedPtr<ColumnVector> &right,
                               SharedPtr<ColumnVector> &result,
                               SizeT count,
                               void *state_ptr,
                               bool nullable) {
        const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
        const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;
        // check constants
        auto left_vector_type = left->vector_type();
        auto right_vector_type = right->vector_type();
        // now only support ColumnVectorType::kConstant and ColumnVectorType::kCompactBit for Boolean
        if (left_vector_type == ColumnVectorType::kConstant && right_vector_type == ColumnVectorType::kConstant) {
            if (!nullable || (left_null->IsAllTrue() && right_null->IsAllTrue())) {
                bool answer;
                Operator::template Execute(left->buffer_->GetCompactBit(0),
                                           right->buffer_->GetCompactBit(0),
                                           answer,
                                           result_null.get(),
                                           0,
                                           state_ptr);
                result->buffer_->SetCompactBit(0, answer);
                result_null->SetAllTrue();
            } else {
                result_null->SetAllFalse();
            }
            result->Finalize(1);
        } else if (left_vector_type == ColumnVectorType::kCompactBit && right_vector_type == ColumnVectorType::kCompactBit) {
            if (!nullable || (left_null->IsAllTrue() && right_null->IsAllTrue())) {
                result_null->SetAllTrue();
                SizeT count_bytes = count / 8;
                SizeT count_tail = count % 8;
                auto left_u8 = reinterpret_cast<const u8 *>(left->data());
                auto right_u8 = reinterpret_cast<const u8 *>(right->data());
                auto result_u8 = reinterpret_cast<u8 *>(result->data());
                for (SizeT i = 0; i < count_bytes; ++i) {
                    Operator::template Execute(left_u8[i], right_u8[i], result_u8[i], result_null.get(), 0, state_ptr);
                }
                if (count_tail > 0) {
                    u8 &tail_u8 = result_u8[count_bytes];
                    u8 ans;
                    Operator::template Execute(left_u8[count_bytes], right_u8[count_bytes], ans, result_null.get(), 0, state_ptr);
                    u8 keep_mask = u8(0xff) << count_tail;
                    tail_u8 = (tail_u8 & keep_mask) | (ans & ~keep_mask);
                }
            } else {
                AllBooleanExecuteWithNull(left, right, result, count, state_ptr);
            }
            result->Finalize(count);
        } else if (left_vector_type == ColumnVectorType::kConstant && right_vector_type == ColumnVectorType::kCompactBit) {
            bool left_value = left->buffer_->GetCompactBit(0);
            u8 left_u8 = left_value ? u8(0xff) : u8(0x00);
            if (nullable && !(left_null->IsAllTrue())) {
                result_null->SetAllFalse();
            } else if (!nullable || (left_null->IsAllTrue() && right_null->IsAllTrue())) {
                result_null->SetAllTrue();
                SizeT count_bytes = count / 8;
                SizeT count_tail = count % 8;
                auto right_u8 = reinterpret_cast<const u8 *>(right->data());
                auto result_u8 = reinterpret_cast<u8 *>(result->data());
                for (SizeT i = 0; i < count_bytes; ++i) {
                    Operator::template Execute(left_u8, right_u8[i], result_u8[i], result_null.get(), 0, state_ptr);
                }
                if (count_tail > 0) {
                    u8 &tail_u8 = result_u8[count_bytes];
                    u8 ans;
                    Operator::template Execute(left_u8, right_u8[count_bytes], ans, result_null.get(), 0, state_ptr);
                    u8 keep_mask = u8(0xff) << count_tail;
                    tail_u8 = (tail_u8 & keep_mask) | (ans & ~keep_mask);
                }
            } else {
                AllBooleanExecuteWithNull(left_u8, right, result, count, state_ptr);
            }
            result->Finalize(count);
        } else if (left_vector_type == ColumnVectorType::kCompactBit && right_vector_type == ColumnVectorType::kConstant) {
            bool right_value = right->buffer_->GetCompactBit(0);
            u8 right_u8 = right_value ? u8(0xff) : u8(0x00);
            if (nullable && !(right_null->IsAllTrue())) {
                result_null->SetAllFalse();
            } else if (!nullable || (left_null->IsAllTrue() && right_null->IsAllTrue())) {
                result_null->SetAllTrue();
                SizeT count_bytes = count / 8;
                SizeT count_tail = count % 8;
                auto left_u8 = reinterpret_cast<const u8 *>(left->data());
                auto result_u8 = reinterpret_cast<u8 *>(result->data());
                for (SizeT i = 0; i < count_bytes; ++i) {
                    Operator::template Execute(left_u8[i], right_u8, result_u8[i], result_null.get(), 0, state_ptr);
                }
                if (count_tail > 0) {
                    u8 &tail_u8 = result_u8[count_bytes];
                    u8 ans;
                    Operator::template Execute(left_u8[count_bytes], right_u8, ans, result_null.get(), 0, state_ptr);
                    u8 keep_mask = u8(0xff) << count_tail;
                    tail_u8 = (tail_u8 & keep_mask) | (ans & ~keep_mask);
                }
            } else {
                AllBooleanExecuteWithNull(left, right_u8, result, count, state_ptr);
            }
            result->Finalize(count);
        } else {
            UnrecoverableError("Wrong boolean operation.");
        }
    }

private:
    static void inline AllBooleanExecuteWithNull(const SharedPtr<ColumnVector> &left,
                                                 const SharedPtr<ColumnVector> &right,
                                                 SharedPtr<ColumnVector> &result,
                                                 SizeT count,
                                                 void *state_ptr) {
        const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
        const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (left_null->IsAllTrue()) {
            result_null->DeepCopy(*right_null);
        } else {
            result_null->DeepCopy(*left_null);
            if (!(right_null->IsAllTrue())) {
                result_null->Merge(*right_null);
            }
        }

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        auto left_u8 = reinterpret_cast<const u8 *>(left->data());
        auto right_u8 = reinterpret_cast<const u8 *>(right->data());
        auto result_u8 = reinterpret_cast<u8 *>(result->data());
        static_assert(BitmaskBuffer::UNIT_BITS % 8 == 0, "static_assert: BitmaskBuffer::UNIT_BITS % 8 == 0");
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            end_index = std::min(end_index, count);
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                const SizeT e = end_index / 8, tail = end_index % 8;
                for (SizeT b = start_index / 8; b < e; ++b) {
                    Operator::template Execute(left_u8[b], right_u8[b], result_u8[b], result_null.get(), 0, state_ptr);
                }
                if (tail) {
                    u8 tail_result;
                    Operator::template Execute(left_u8[e], right_u8[e], tail_result, result_null.get(), 0, state_ptr);
                    const u8 mask_keep = u8(0xff) << tail;
                    result_u8[e] = (result_u8[e] & mask_keep) | (tail_result & ~mask_keep);
                }
                start_index = end_index;
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                start_index = end_index;
            } else {
                for (BooleanT answer; start_index < end_index; ++start_index) {
                    if (result_null->IsTrue(start_index)) {
                        // This row isn't null
                        Operator::template Execute(left->buffer_->GetCompactBit(start_index),
                                                   right->buffer_->GetCompactBit(start_index),
                                                   answer,
                                                   result_null.get(),
                                                   start_index,
                                                   state_ptr);
                        result->buffer_->SetCompactBit(start_index, answer);
                    }
                }
            }
        }
    }

    static void inline AllBooleanExecuteWithNull(const SharedPtr<ColumnVector> &left,
                                                 const u8 right_u8,
                                                 SharedPtr<ColumnVector> &result,
                                                 SizeT count,
                                                 void *state_ptr) {
        const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        result_null->DeepCopy(*left_null);

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        auto left_u8 = reinterpret_cast<const u8 *>(left->data());
        BooleanT right_boolean = (right_u8 & (u8(1)));
        auto result_u8 = reinterpret_cast<u8 *>(result->data());
        static_assert(BitmaskBuffer::UNIT_BITS % 8 == 0, "static_assert: BitmaskBuffer::UNIT_BITS % 8 == 0");
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            end_index = std::min(end_index, count);
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                const SizeT e = end_index / 8, tail = end_index % 8;
                for (SizeT b = start_index / 8; b < e; ++b) {
                    Operator::template Execute(left_u8[b], right_u8, result_u8[b], result_null.get(), 0, state_ptr);
                }
                if (tail) {
                    u8 tail_result;
                    Operator::template Execute(left_u8[e], right_u8, tail_result, result_null.get(), 0, state_ptr);
                    const u8 mask_keep = u8(0xff) << tail;
                    result_u8[e] = (result_u8[e] & mask_keep) | (tail_result & ~mask_keep);
                }
                start_index = end_index;
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                start_index = end_index;
            } else {
                for (BooleanT answer; start_index < end_index; ++start_index) {
                    if (result_null->IsTrue(start_index)) {
                        // This row isn't null
                        Operator::template Execute(left->buffer_->GetCompactBit(start_index),
                                                   right_boolean,
                                                   answer,
                                                   result_null.get(),
                                                   start_index,
                                                   state_ptr);
                        result->buffer_->SetCompactBit(start_index, answer);
                    }
                }
            }
        }
    }

    static void inline AllBooleanExecuteWithNull(const u8 left_u8,
                                                 const SharedPtr<ColumnVector> &right,
                                                 SharedPtr<ColumnVector> &result,
                                                 SizeT count,
                                                 void *state_ptr) {
        const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        result_null->DeepCopy(*right_null);

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        auto right_u8 = reinterpret_cast<const u8 *>(right->data());
        BooleanT left_boolean = (left_u8 & (u8(1)));
        auto result_u8 = reinterpret_cast<u8 *>(result->data());
        static_assert(BitmaskBuffer::UNIT_BITS % 8 == 0, "static_assert: BitmaskBuffer::UNIT_BITS % 8 == 0");
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            end_index = std::min(end_index, count);
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                const SizeT e = end_index / 8, tail = end_index % 8;
                for (SizeT b = start_index / 8; b < e; ++b) {
                    Operator::template Execute(left_u8, right_u8[b], result_u8[b], result_null.get(), 0, state_ptr);
                }
                if (tail) {
                    u8 tail_result;
                    Operator::template Execute(left_u8, right_u8[e], tail_result, result_null.get(), 0, state_ptr);
                    const u8 mask_keep = u8(0xff) << tail;
                    result_u8[e] = (result_u8[e] & mask_keep) | (tail_result & ~mask_keep);
                }
                start_index = end_index;
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                start_index = end_index;
            } else {
                for (BooleanT answer; start_index < end_index; ++start_index) {
                    if (result_null->IsTrue(start_index)) {
                        // This row isn't null
                        Operator::template Execute(left_boolean,
                                                   right->buffer_->GetCompactBit(start_index),
                                                   answer,
                                                   result_null.get(),
                                                   start_index,
                                                   state_ptr);
                        result->buffer_->SetCompactBit(start_index, answer);
                    }
                }
            }
        }
    }
};

export class BinaryOperator {
public:
    // TODO: MixedType is not handled now.
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
        requires IsAnyOf<MixedT, LeftType, RightType, ResultType>
    static void inline Execute(const SharedPtr<ColumnVector> &left,
                               const SharedPtr<ColumnVector> &right,
                               SharedPtr<ColumnVector> &result,
                               SizeT count,
                               void *state_ptr,
                               bool nullable) {
        UnrecoverableError("MixedType needs to be specialized.");
    }

    // case for BinaryOperator which returns BooleanT result
    template <BinaryGenerateBoolean LeftType, BinaryGenerateBoolean RightType, std::same_as<BooleanT> ResultType, typename Operator>
    static void inline Execute(const SharedPtr<ColumnVector> &left,
                               const SharedPtr<ColumnVector> &right,
                               SharedPtr<ColumnVector> &result,
                               SizeT count,
                               void *state_ptr,
                               bool nullable) {
        return BooleanResultBinaryOperator<LeftType, RightType, Operator>::Execute(left, right, result, count, state_ptr, nullable);
    }

    // case for type which can be accessed by type_ptr[i]
    template <PODValueType LeftType, PODValueType RightType, PODValueType ResultType, typename Operator>
    static void inline Execute(const SharedPtr<ColumnVector> &left,
                               const SharedPtr<ColumnVector> &right,
                               SharedPtr<ColumnVector> &result,
                               SizeT count,
                               void *state_ptr,
                               bool nullable) {
        switch (left->vector_type()) {
            case ColumnVectorType::kInvalid: {
                UnrecoverableError("Invalid column vector type.");
            }
            case ColumnVectorType::kCompactBit: {
                UnrecoverableError("kCompactBit should not be in this branch.");
            }
            case ColumnVectorType::kFlat: {
                return ExecuteFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteConstant<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneous<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
        }
    }

private:
    // Level-1 switch case functions
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlat(const SharedPtr<ColumnVector> &left,
                                   const SharedPtr<ColumnVector> &right,
                                   SharedPtr<ColumnVector> &result,
                                   SizeT count,
                                   void *state_ptr,
                                   bool nullable) {

        switch (right->vector_type()) {
            case ColumnVectorType::kInvalid:
                UnrecoverableError("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteFlatFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteFlatConstant<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteFlatHeterogeneous<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kCompactBit: {
                UnrecoverableError("CompactBit isn't implemented.");
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstant(const SharedPtr<ColumnVector> &left,
                                       const SharedPtr<ColumnVector> &right,
                                       SharedPtr<ColumnVector> &result,
                                       SizeT count,
                                       void *state_ptr,
                                       bool nullable) {

        switch (right->vector_type()) {
            case ColumnVectorType::kInvalid:
                UnrecoverableError("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteConstantFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteConstantConstant<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteConstantHeterogeneous<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kCompactBit: {
                UnrecoverableError("CompactBit isn't implemented.");
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneous(const SharedPtr<ColumnVector> &left,
                                            const SharedPtr<ColumnVector> &right,
                                            SharedPtr<ColumnVector> &result,
                                            SizeT count,
                                            void *state_ptr,
                                            bool nullable) {

        switch (right->vector_type()) {
            case ColumnVectorType::kInvalid:
                UnrecoverableError("Invalid column vector type.");
            case ColumnVectorType::kFlat: {
                return ExecuteHeterogeneousFlat<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kConstant: {
                return ExecuteHeterogeneousConstant<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kHeterogeneous: {
                return ExecuteHeterogeneousHeterogeneous<LeftType, RightType, ResultType, Operator>(left, right, result, count, state_ptr, nullable);
            }
            case ColumnVectorType::kCompactBit: {
                UnrecoverableError("CompactBit isn't implemented.");
            }
        }
    }

private:
    // Level-2 switch case function
    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatFlat(const SharedPtr<ColumnVector> &left,
                                       const SharedPtr<ColumnVector> &right,
                                       SharedPtr<ColumnVector> &result,
                                       SizeT count,
                                       void *state_ptr,
                                       bool nullable) {

        const auto *left_ptr = (const LeftType *)(left->data());
        const auto *right_ptr = (const RightType *)(right->data());
        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (nullable) {
            const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
            const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;

            ExecuteFlatFlatWithNull<LeftType, RightType, ResultType, Operator>(left_ptr,
                                                                               left_null,
                                                                               right_ptr,
                                                                               right_null,
                                                                               result_ptr,
                                                                               result_null,
                                                                               count,
                                                                               state_ptr);

        } else {
            result_null->SetAllTrue();

            for (SizeT i = 0; i < count; i++) {
                Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[i],
                                                                            right_ptr[i],
                                                                            result_ptr[i],
                                                                            result_null.get(),
                                                                            i,
                                                                            state_ptr);
            }
        }
        result->Finalize(count);
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatFlatWithNull(const LeftType *__restrict left_ptr,
                                               const SharedPtr<Bitmask> &left_null,
                                               const RightType *__restrict right_ptr,
                                               const SharedPtr<Bitmask> &right_null,
                                               ResultType *__restrict result_ptr,
                                               SharedPtr<Bitmask> &result_null,
                                               SizeT count,
                                               void *state_ptr) {

        if (left_null->IsAllTrue()) {
            if (right_null->IsAllTrue()) {
                // Initialized all true to output null bitmask.
                result_null->SetAllTrue();

                for (SizeT i = 0; i < count; i++) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[i],
                                                                                right_ptr[i],
                                                                                result_ptr[i],
                                                                                result_null.get(),
                                                                                i,
                                                                                state_ptr);
                }

                return;
            } else {
                result_null->DeepCopy(*right_null);
            }
        } else {
            if (right_null->IsAllTrue()) {
                result_null->DeepCopy(*left_null);
            } else {
                result_null->DeepCopy(*right_null);
                result_null->Merge(*left_null);
            }
        }

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                while (start_index < end_index) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[start_index],
                                                                                right_ptr[start_index],
                                                                                result_ptr[start_index],
                                                                                result_null.get(),
                                                                                start_index,
                                                                                state_ptr);
                    ++start_index;
                }
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                ;
            } else {
                SizeT original_start = start_index;
                while (start_index < end_index) {
                    if (result_null->IsTrue(start_index - original_start)) {
                        // This row isn't null
                        Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[start_index],
                                                                                    right_ptr[start_index],
                                                                                    result_ptr[start_index],
                                                                                    result_null.get(),
                                                                                    start_index,
                                                                                    state_ptr);
                        ++start_index;
                    }
                }
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatConstant(const SharedPtr<ColumnVector> &left,
                                           const SharedPtr<ColumnVector> &right,
                                           SharedPtr<ColumnVector> &result,
                                           SizeT count,
                                           void *state_ptr,
                                           bool nullable) {

        const auto *left_ptr = (const LeftType *)(left->data());
        const auto *right_ptr = (const RightType *)(right->data());
        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (nullable) {
            const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
            const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;

            ExecuteFlatConstantWithNull<LeftType, RightType, ResultType, Operator>(left_ptr,
                                                                                   left_null,
                                                                                   right_ptr,
                                                                                   right_null,
                                                                                   result_ptr,
                                                                                   result_null,
                                                                                   count,
                                                                                   state_ptr);

        } else {
            result_null->SetAllTrue();

            for (SizeT i = 0; i < count; i++) {
                Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[i],
                                                                            right_ptr[0],
                                                                            result_ptr[i],
                                                                            result_null.get(),
                                                                            i,
                                                                            state_ptr);
            }
        }
        result->Finalize(count);
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatConstantWithNull(const LeftType *__restrict left_ptr,
                                                   const SharedPtr<Bitmask> &left_null,
                                                   const RightType *__restrict right_ptr,
                                                   const SharedPtr<Bitmask> &right_null,
                                                   ResultType *__restrict result_ptr,
                                                   SharedPtr<Bitmask> &result_null,
                                                   SizeT count,
                                                   void *state_ptr) {

        if (left_null->IsAllTrue()) {
            if (right_null->IsAllTrue()) {
                // Initialized all true to output null bitmask.
                result_null->SetAllTrue();

                for (SizeT i = 0; i < count; i++) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[i],
                                                                                right_ptr[0],
                                                                                result_ptr[i],
                                                                                result_null.get(),
                                                                                i,
                                                                                state_ptr);
                }

                return;
            } else {
                // Set all result all null, and return directly
                result_null->SetAllFalse();
            }
        } else {
            if (right_null->IsAllTrue()) {
                result_null->DeepCopy(*left_null);
            } else {
                // Set all result all null, and return directly
                result_null->SetAllFalse();
            }
        }

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                while (start_index < end_index) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[start_index],
                                                                                right_ptr[0],
                                                                                result_ptr[start_index],
                                                                                result_null.get(),
                                                                                start_index,
                                                                                state_ptr);
                    ++start_index;
                }
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                ;
            } else {
                SizeT original_start = start_index;
                while (start_index < end_index) {
                    if (result_null->IsTrue(start_index - original_start)) {
                        // This row isn't null
                        Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[start_index],
                                                                                    right_ptr[0],
                                                                                    result_ptr[start_index],
                                                                                    result_null.get(),
                                                                                    start_index,
                                                                                    state_ptr);
                        ++start_index;
                    }
                }
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteFlatHeterogeneous(const SharedPtr<ColumnVector> &,
                                                const SharedPtr<ColumnVector> &,
                                                SharedPtr<ColumnVector> &,
                                                SizeT,
                                                void *,
                                                bool) {
        RecoverableError(Status::NotSupport("Not implemented"));
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantFlat(const SharedPtr<ColumnVector> &left,
                                           const SharedPtr<ColumnVector> &right,
                                           SharedPtr<ColumnVector> &result,
                                           SizeT count,
                                           void *state_ptr,
                                           bool nullable) {

        const auto *left_ptr = (const LeftType *)(left->data());
        const auto *right_ptr = (const RightType *)(right->data());
        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (nullable) {
            const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
            const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;

            ExecuteConstantFlatWithNull<LeftType, RightType, ResultType, Operator>(left_ptr,
                                                                                   left_null,
                                                                                   right_ptr,
                                                                                   right_null,
                                                                                   result_ptr,
                                                                                   result_null,
                                                                                   count,
                                                                                   state_ptr);

        } else {
            result_null->SetAllTrue();

            for (SizeT i = 0; i < count; i++) {
                Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                            right_ptr[i],
                                                                            result_ptr[i],
                                                                            result_null.get(),
                                                                            i,
                                                                            state_ptr);
            }
        }
        result->Finalize(count);
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantFlatWithNull(const LeftType *__restrict left_ptr,
                                                   const SharedPtr<Bitmask> &left_null,
                                                   const RightType *__restrict right_ptr,
                                                   const SharedPtr<Bitmask> &right_null,
                                                   ResultType *__restrict result_ptr,
                                                   SharedPtr<Bitmask> &result_null,
                                                   SizeT count,
                                                   void *state_ptr) {

        if (right_null->IsAllTrue()) {
            if (left_null->IsAllTrue()) {
                // Initialized all true to output null bitmask.
                result_null->SetAllTrue();

                for (SizeT i = 0; i < count; i++) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                                right_ptr[i],
                                                                                result_ptr[i],
                                                                                result_null.get(),
                                                                                i,
                                                                                state_ptr);
                }

                return;
            } else {
                // Set all result all null, and return directly
                result_null->SetAllFalse();
            }
        } else {
            if (left_null->IsAllTrue()) {
                result_null->DeepCopy(*right_null);
            } else {
                // Set all result all null, and return directly
                result_null->SetAllFalse();
            }
        }

        const u64 *result_null_data = result_null->GetData();
        SizeT unit_count = BitmaskBuffer::UnitCount(count);
        for (SizeT i = 0, start_index = 0, end_index = BitmaskBuffer::UNIT_BITS; i < unit_count; ++i, end_index += BitmaskBuffer::UNIT_BITS) {
            if (result_null_data[i] == BitmaskBuffer::UNIT_MAX) {
                // all data of 64 rows are not null
                while (start_index < end_index) {
                    Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                                right_ptr[start_index],
                                                                                result_ptr[start_index],
                                                                                result_null.get(),
                                                                                start_index,
                                                                                state_ptr);
                    ++start_index;
                }
            } else if (result_null_data[i] == BitmaskBuffer::UNIT_MIN) {
                // all data of 64 rows are null
                ;
            } else {
                SizeT original_start = start_index;
                while (start_index < end_index) {
                    if (result_null->IsTrue(start_index - original_start)) {
                        // This row isn't null
                        Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                                    right_ptr[start_index],
                                                                                    result_ptr[start_index],
                                                                                    result_null.get(),
                                                                                    start_index,
                                                                                    state_ptr);
                        ++start_index;
                    }
                }
            }
        }
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantConstant(const SharedPtr<ColumnVector> &left,
                                               const SharedPtr<ColumnVector> &right,
                                               SharedPtr<ColumnVector> &result,
                                               SizeT,
                                               void *state_ptr,
                                               bool nullable) {

        const auto *left_ptr = (const LeftType *)(left->data());
        const auto *right_ptr = (const RightType *)(right->data());
        auto *result_ptr = (ResultType *)(result->data());
        SharedPtr<Bitmask> &result_null = result->nulls_ptr_;

        if (nullable) {
            const SharedPtr<Bitmask> &left_null = left->nulls_ptr_;
            const SharedPtr<Bitmask> &right_null = right->nulls_ptr_;

            if (left_null->IsAllTrue() && right_null->IsAllTrue()) {
                Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0],
                                                                            right_ptr[0],
                                                                            result_ptr[0],
                                                                            result_null.get(),
                                                                            0,
                                                                            state_ptr);
            } else {
                result_null->SetAllFalse();
            }
        } else {
            result_null->SetAllTrue();
            Operator::template Execute<LeftType, RightType, ResultType>(left_ptr[0], right_ptr[0], result_ptr[0], result_null.get(), 0, state_ptr);
        }
        result->Finalize(1);
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteConstantHeterogeneous(const SharedPtr<ColumnVector> &,
                                                    const SharedPtr<ColumnVector> &,
                                                    SharedPtr<ColumnVector> &,
                                                    SizeT,
                                                    void *,
                                                    bool) {
        RecoverableError(Status::NotSupport("Not implemented"));
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousFlat(const SharedPtr<ColumnVector> &,
                                                const SharedPtr<ColumnVector> &,
                                                SharedPtr<ColumnVector> &,
                                                SizeT,
                                                void *,
                                                bool) {
        RecoverableError(Status::NotSupport("Not implemented"));
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousConstant(const SharedPtr<ColumnVector> &,
                                                    const SharedPtr<ColumnVector> &,
                                                    SharedPtr<ColumnVector> &,
                                                    SizeT,
                                                    void *,
                                                    bool) {
        RecoverableError(Status::NotSupport("Not implemented"));
    }

    template <typename LeftType, typename RightType, typename ResultType, typename Operator>
    static void inline ExecuteHeterogeneousHeterogeneous(const SharedPtr<ColumnVector> &,
                                                         const SharedPtr<ColumnVector> &,
                                                         SharedPtr<ColumnVector> &,
                                                         SizeT,
                                                         void *,
                                                         bool) {
        RecoverableError(Status::NotSupport("Not implemented"));
    }
};

} // namespace infinity
