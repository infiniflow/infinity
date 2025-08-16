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

export module infinity_core:secondary_index_pgm;

import :local_file_handle;
import :infinity_exception;

import third_party;

namespace infinity {

struct SecondaryIndexApproxPos {
    size_t pos_{};         ///< The approximate position of the key.
    size_t lower_bound_{}; ///< The lower bound of the range.
    size_t upper_bound_{}; ///< The upper bound of the range.
};

template <typename T>
struct PGMT;

template <typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, float>
struct PGMT<T> {
    using type = PGMIndex<T, 64, 4, float>;
};

template <typename T>
    requires std::is_same_v<T, double>
struct PGMT<T> {
    using type = PGMIndex<T, 64, 4, double>;
};

template <typename IndexValueType>
using PGMIndexType = PGMT<IndexValueType>::type;

// PGMIndex member objects:
// size_t n;                           ///< The number of elements this index was built on.
// K first_key;                        ///< The smallest element. K is the type of key.
// std::vector<Segment> segments;      ///< The segments composing the index.
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

    inline void Load(LocalFileHandle &file_handle) {
        {
            // load n
            u32 save_n;
            file_handle.Read(&save_n, sizeof(save_n));
            this->n = save_n;
        }
        {
            // load first_key
            IndexValueType save_first_key;
            file_handle.Read(&save_first_key, sizeof(save_first_key));
            this->first_key = save_first_key;
        }
        {
            // load std::vector<Segment> segments
            u32 save_size;
            file_handle.Read(&save_size, sizeof(save_size));
            this->segments.resize(save_size);
            file_handle.Read(this->segments.data(), save_size * sizeof(typename decltype(this->segments)::value_type));
        }
        {
            // load std::vector<size_t> levels_offsets
            u32 save_levels_offsets_size;
            file_handle.Read(&save_levels_offsets_size, sizeof(save_levels_offsets_size));
            this->levels_offsets.resize(save_levels_offsets_size);
            file_handle.Read(this->levels_offsets.data(), save_levels_offsets_size * sizeof(typename decltype(this->levels_offsets)::value_type));
        }
    }

    inline void Save(LocalFileHandle &file_handle) const {
        {
            // save n
            u32 save_n = this->n;
            file_handle.Append(&save_n, sizeof(save_n));
        }
        {
            // save first_key
            IndexValueType save_first_key = this->first_key;
            file_handle.Append(&save_first_key, sizeof(save_first_key));
        }
        {
            // save std::vector<Segment> segments
            u32 save_size = this->segments.size();
            file_handle.Append(&save_size, sizeof(save_size));
            file_handle.Append(this->segments.data(), save_size * sizeof(typename decltype(this->segments)::value_type));
        }
        {
            // save std::vector<size_t> levels_offsets
            u32 save_levels_offsets_size = this->levels_offsets.size();
            file_handle.Append(&save_levels_offsets_size, sizeof(save_levels_offsets_size));
            file_handle.Append(this->levels_offsets.data(), save_levels_offsets_size * sizeof(typename decltype(this->levels_offsets)::value_type));
        }
    }
};

export class SecondaryPGMIndex {
public:
    virtual ~SecondaryPGMIndex() = default;

    virtual void SaveIndex(LocalFileHandle &file_handle) const = 0;

    virtual void LoadIndex(LocalFileHandle &file_handle) = 0;

    virtual void BuildIndex(size_t data_cnt, const void *data_ptr) = 0;

    virtual SecondaryIndexApproxPos SearchIndex(const void *val_ptr) const = 0;
};

template <typename IndexValueType>
class SecondaryPGMIndexTemplate final : public SecondaryPGMIndex {
    std::unique_ptr<PGMWithExtraFunction<IndexValueType>> pgm_index_;
    bool initialized_{false};

public:
    SecondaryPGMIndexTemplate() = default;

    ~SecondaryPGMIndexTemplate() override = default;

    void SaveIndex(LocalFileHandle &file_handle) const override {
        if (!initialized_) {
            UnrecoverableError("Not initialized yet.");
        }
        pgm_index_->Save(file_handle);
    }

    void LoadIndex(LocalFileHandle &file_handle) override {
        if (initialized_) {
            UnrecoverableError("Already initialized.");
        }
        pgm_index_ = std::make_unique<PGMWithExtraFunction<IndexValueType>>();
        pgm_index_->Load(file_handle);
        initialized_ = true;
    }

    void BuildIndex(size_t data_cnt, const void *data_ptr) override {
        if (initialized_) {
            UnrecoverableError("Already initialized.");
        }
        auto typed_data_ptr = static_cast<const IndexValueType *>(data_ptr);
        pgm_index_ = std::make_unique<PGMWithExtraFunction<IndexValueType>>(typed_data_ptr, typed_data_ptr + data_cnt);
        initialized_ = true;
    }

    SecondaryIndexApproxPos SearchIndex(const void *val_ptr) const override {
        if (!initialized_) {
            UnrecoverableError("Not initialized yet.");
        }
        auto val = *(static_cast<const IndexValueType *>(val_ptr));
        auto [pos, lo, hi] = pgm_index_->search(val);
        return {pos, lo, hi};
    }
};

export template <typename IndexValueType>
inline std::unique_ptr<SecondaryPGMIndex> GenerateSecondaryPGMIndex() {
    return std::make_unique<SecondaryPGMIndexTemplate<IndexValueType>>();
}

} // namespace infinity
