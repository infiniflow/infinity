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

#include "parser/type/datetime/date_type.h"
#include "parser/type/datetime/datetime_type.h"
#include "parser/type/datetime/time_type.h"
#include "parser/type/datetime/timestamp_type.h"
#include "pgm/pgm_index.hpp"

import stl;
import third_party;
import data_block;
import column_vector;
import buffer_manager;
import default_values;
import data_block;
import buffer_handle;
import local_file_system;
import catalog;
import parser;
import codec;
import file_writer;
import file_reader;

module pgm_numeric;

namespace infinity {

template <typename VALUE>
class PGM : public PGMIndex<VALUE, 8>, public PGMBase {
    using BASE = PGMIndex<VALUE, 8>;
    using BASE::BASE;

    void AppendBlock(const void *data, SizeT size) final;

    void Build() final;

    ApproxPos Search(u64 val) const final;

    void Load(SharedPtr<FileReader> &file_reader) final;

    void Save(SharedPtr<FileWriter> &file_writer) const final;

private:
    Vector<VALUE> data_buffer_;
};

template <typename VALUE>
void PGM<VALUE>::AppendBlock(const void *data, SizeT size) {
    const VALUE *data_ptr = reinterpret_cast<const VALUE *>(data);
    data_buffer_.insert(data_buffer_.end(), data_ptr, data_ptr + size);
}

template <typename VALUE>
void PGM<VALUE>::Build() {
    BASE(data_buffer_.begin(), data_buffer_.end());
    data_buffer_.clear();
}

template <typename VALUE>
void PGM<VALUE>::Load(SharedPtr<FileReader> &file_reader) {
    this->n = file_reader->ReadVInt();
    this->first_key = file_reader->ReadVLong();
    auto size = file_reader->ReadVInt();
    this->segments.resize(size);
    for (auto &segment : this->segments) {
        segment.key = file_reader->ReadVLong();
        auto s = file_reader->ReadVInt();
        segment.slope = Codec::U32ToFloat(s);
        segment.intercept = file_reader->ReadVInt();
    }
    size = file_reader->ReadVInt();
    this->levels_sizes.resize(size);
    for (size_t &level_size : this->levels_sizes)
        level_size = file_reader->ReadVLong();
    auto s = file_reader->ReadVInt();
    this->levels_offsets.resize(s);
    for (size_t &level_offset : this->levels_offsets)
        level_offset = file_reader->ReadVLong();
}

template <typename VALUE>
void PGM<VALUE>::Save(SharedPtr<FileWriter> &file_writer) const {
    file_writer->WriteVInt((u32)(this->n));
    file_writer->WriteVLong((u64)(this->first_key));
    file_writer->WriteVInt((u32)(this->segments.size()));

    for (const auto &segment : this->segments) {
        file_writer->WriteVLong((u64)(segment.key));
        file_writer->WriteVInt(Codec::FloatToU32(segment.slope));
        file_writer->WriteVInt(segment.intercept);
    }

    file_writer->WriteVInt((u32)(this->levels_sizes.size()));
    for (const SizeT &level_size : this->levels_sizes)
        file_writer->WriteVLong(level_size);

    file_writer->WriteVInt((u32)(this->levels_offsets.size()));
    for (const SizeT &level_offset : this->levels_offsets)
        file_writer->WriteVLong(level_offset);
}

static ApproxPos GetPos(pgm::ApproxPos approx_pos) {
    ApproxPos pos_it;
    pos_it.pos_ = approx_pos.pos;
    pos_it.lower_bound_ = approx_pos.lo;
    pos_it.upper_bound_ = approx_pos.hi;
    return pos_it;
}

template <>
inline ApproxPos PGM<u8>::Search(u64 val) const {
    return GetPos(this->search((u8)val));
}

template <>
inline ApproxPos PGM<u16>::Search(u64 val) const {
    return GetPos(this->search((u16)val));
}

template <>
inline ApproxPos PGM<u32>::Search(u64 val) const {
    return GetPos(this->search((u32)val));
}

template <>
inline ApproxPos PGM<u64>::Search(u64 val) const {
    return GetPos(this->search(val));
}

template <>
inline ApproxPos PGM<float>::Search(u64 val) const {
    return GetPos(this->search(Codec::U32ToFloat((u32)val)));
}

template <>
inline ApproxPos PGM<double>::Search(u64 val) const {
    return GetPos(this->search(Codec::U64ToDouble(val)));
}

template <>
inline ApproxPos PGM<i32>::Search(u64 val) const {
    return GetPos(this->search((i32)val));
}

template <>
inline ApproxPos PGM<i64>::Search(u64 val) const {
    return GetPos(this->search((i64)val));
}

NumericIndex::NumericIndex() {}

void NumericIndex::CreatePGM(LogicalType logical_type) {
    switch (logical_type) {
        case kTinyInt: {
            column_index_.reset(new PGM<u8>());
        } break;
        case kSmallInt: {
            column_index_.reset(new PGM<u16>());
        } break;
        case kInteger: {
            column_index_.reset(new PGM<u32>());
        } break;
        case kBigInt: {
            column_index_.reset(new PGM<u64>());
        } break;
        case kFloat: {
            column_index_.reset(new PGM<float>());
        } break;
        case kDouble: {
            column_index_.reset(new PGM<double>());
        } break;
        case kDate: {
            column_index_.reset(new PGM<i32>());
        } break;
        case kTime: {
            column_index_.reset(new PGM<i32>());
        } break;
        case kDateTime: {
            column_index_.reset(new PGM<i64>());
        } break;
        case kTimestamp: {
            column_index_.reset(new PGM<i64>());
        } break;
        default:
            break;
    }
}

void NumericIndex::Insert(SegmentEntry *segment_entry, SharedPtr<ColumnDef> column_def, BufferManager *buffer_mgr) {
    u64 column_id = column_def->id();
    const auto& block_entries = segment_entry->block_entries();
    if (!column_index_.get()) {
        CreatePGM(block_entries[0]->GetColumnBlockEntry(column_id)->column_type()->type());
    }

    for (const auto &block_entry : block_entries) {
        for (SizeT i = 0; i < block_entry->row_count(); ++i) {
            auto block_column_entry = block_entry->GetColumnBlockEntry(column_id);
            BufferHandle buffer_handle = block_column_entry->buffer()->Load();
            switch (block_column_entry->column_type()->type()) {
                case kTinyInt: {
                    auto block_data_ptr = reinterpret_cast<const TinyIntT *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                case kSmallInt: {
                    auto block_data_ptr = reinterpret_cast<const SmallIntT *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                case kInteger: {
                    auto block_data_ptr = reinterpret_cast<const IntegerT *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                case kBigInt: {
                    auto block_data_ptr = reinterpret_cast<const BigIntT *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                case kFloat: {
                    auto block_data_ptr = reinterpret_cast<const FloatT *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                case kDouble: {
                    auto block_data_ptr = reinterpret_cast<const DoubleT *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                case kDate: {
                    auto block_data_ptr = reinterpret_cast<const DateType *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                case kTime: {
                    auto block_data_ptr = reinterpret_cast<const TimeType *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                case kDateTime: {
                    auto block_data_ptr = reinterpret_cast<const DateTimeType *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                case kTimestamp: {
                    auto block_data_ptr = reinterpret_cast<const TimestampType *>(buffer_handle.GetData());
                    column_index_->AppendBlock(block_data_ptr, block_entry->row_count());
                } break;
                default:
                    break;
            }
        }
    }
    column_index_->Build();
}

} // namespace infinity