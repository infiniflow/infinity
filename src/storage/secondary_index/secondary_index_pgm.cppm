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

export module secondary_index_pgm;

import stl;

import third_party;
import file_system;
import infinity_exception;

namespace infinity {

struct SecondaryIndexApproxPos {
    SizeT pos_{};         ///< The approximate position of the key.
    SizeT lower_bound_{}; ///< The lower bound of the range.
    SizeT upper_bound_{}; ///< The upper bound of the range.
};

template <typename IndexValueType>
using PGMIndexType = PGMIndex<IndexValueType, 8>;

// PGMIndex member objects:
// size_t n;                           ///< The number of elements this index was built on.
// K first_key;                        ///< The smallest element. K is the type of key.
// std::vector<Segment> segments;      ///< The segments composing the index.
// std::vector<size_t> levels_sizes;   ///< The number of segment in each level, in reverse order.
// std::vector<size_t> levels_offsets; ///< The starting position of each level in segments[], in reverse order.

// Segment member objects:
// K key;             ///< The first key that the segment indexes.
// Floating slope;    ///< The slope of the segment.
// int32_t intercept; ///< The intercept of the segment.

template <typename IndexValueType>
class PGMWithExtraFunction final : public PGMIndexType<IndexValueType> {
public:
    PGMWithExtraFunction() = default;

    // call PGMIndexType constructor
    // input data in range (first, last) need to be sorted
    template <typename RandomIt>
    PGMWithExtraFunction(RandomIt first, RandomIt last) : PGMIndexType<IndexValueType>(first, last) {}

    inline void Load(FileHandler &file_handler) {
        {
            // load n
            u32 save_n;
            file_handler.Read(&save_n, sizeof(save_n));
            this->n = save_n;
        }
        {
            // load first_key
            IndexValueType save_first_key;
            file_handler.Read(&save_first_key, sizeof(save_first_key));
            this->first_key = save_first_key;
        }
        {
            // load std::vector<Segment> segments
            u32 save_size;
            file_handler.Read(&save_size, sizeof(save_size));
            this->segments.resize(save_size);
            file_handler.Read(this->segments.data(), save_size * sizeof(typename decltype(this->segments)::value_type));
        }
        {
            // load std::vector<size_t> levels_sizes
            u32 save_levels_sizes_size;
            file_handler.Read(&save_levels_sizes_size, sizeof(save_levels_sizes_size));
            this->levels_sizes.resize(save_levels_sizes_size);
            file_handler.Read(this->levels_sizes.data(), save_levels_sizes_size * sizeof(typename decltype(this->levels_sizes)::value_type));
        }
        {
            // load std::vector<size_t> levels_offsets
            u32 save_levels_offsets_size;
            file_handler.Read(&save_levels_offsets_size, sizeof(save_levels_offsets_size));
            this->levels_offsets.resize(save_levels_offsets_size);
            file_handler.Read(this->levels_offsets.data(), save_levels_offsets_size * sizeof(typename decltype(this->levels_offsets)::value_type));
        }
    }

    inline void Save(FileHandler &file_handler) const {
        {
            // save n
            u32 save_n = this->n;
            file_handler.Write(&save_n, sizeof(save_n));
        }
        {
            // save first_key
            IndexValueType save_first_key = this->first_key;
            file_handler.Write(&save_first_key, sizeof(save_first_key));
        }
        {
            // save std::vector<Segment> segments
            u32 save_size = this->segments.size();
            file_handler.Write(&save_size, sizeof(save_size));
            file_handler.Write(this->segments.data(), save_size * sizeof(typename decltype(this->segments)::value_type));
        }
        {
            // save std::vector<size_t> levels_sizes
            u32 save_levels_sizes_size = this->levels_sizes.size();
            file_handler.Write(&save_levels_sizes_size, sizeof(save_levels_sizes_size));
            file_handler.Write(this->levels_sizes.data(), save_levels_sizes_size * sizeof(typename decltype(this->levels_sizes)::value_type));
        }
        {
            // save std::vector<size_t> levels_offsets
            u32 save_levels_offsets_size = this->levels_offsets.size();
            file_handler.Write(&save_levels_offsets_size, sizeof(save_levels_offsets_size));
            file_handler.Write(this->levels_offsets.data(), save_levels_offsets_size * sizeof(typename decltype(this->levels_offsets)::value_type));
        }
    }
};

export class SecondaryPGMIndex {
public:
    virtual ~SecondaryPGMIndex() = default;

    virtual void SaveIndex(FileHandler &file_handler) const = 0;

    virtual void LoadIndex(FileHandler &file_handler) = 0;

    virtual void BuildIndex(SizeT data_cnt, const void *data_ptr) = 0;

    virtual SecondaryIndexApproxPos SearchIndex(const void *val_ptr) const = 0;
};

template <typename IndexValueType>
class SecondaryPGMIndexTemplate final : public SecondaryPGMIndex {
    UniquePtr<PGMWithExtraFunction<IndexValueType>> pgm_index_;
    bool initialized_{false};

public:
    SecondaryPGMIndexTemplate() = default;

    ~SecondaryPGMIndexTemplate() final = default;

    void SaveIndex(FileHandler &file_handler) const final {
        if (!initialized_) {
            UnrecoverableError("Not initialized yet.");
        }
        pgm_index_->Save(file_handler);
    }

    void LoadIndex(FileHandler &file_handler) final {
        if (initialized_) {
            UnrecoverableError("Already initialized.");
        }
        pgm_index_ = MakeUnique<PGMWithExtraFunction<IndexValueType>>();
        pgm_index_->Load(file_handler);
        initialized_ = true;
    }

    void BuildIndex(SizeT data_cnt, const void *data_ptr) final {
        if (initialized_) {
            UnrecoverableError("Already initialized.");
        }
        auto typed_data_ptr = static_cast<const IndexValueType *>(data_ptr);
        pgm_index_ = MakeUnique<PGMWithExtraFunction<IndexValueType>>(typed_data_ptr, typed_data_ptr + data_cnt);
        initialized_ = true;
    }

    SecondaryIndexApproxPos SearchIndex(const void *val_ptr) const final {
        if (!initialized_) {
            UnrecoverableError("Not initialized yet.");
        }
        auto val = *(static_cast<const IndexValueType *>(val_ptr));
        auto [pos, lo, hi] = pgm_index_->search(val);
        return {pos, lo, hi};
    }
};

export template <typename IndexValueType>
inline UniquePtr<SecondaryPGMIndex> GenerateSecondaryPGMIndex() {
    return MakeUnique<SecondaryPGMIndexTemplate<IndexValueType>>();
}

} // namespace infinity