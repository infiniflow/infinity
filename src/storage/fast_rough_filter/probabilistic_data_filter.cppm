//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

export module probabilistic_data_filter;
import stl;
import internal_types;
import value;
import binary_fuse_filter;
import logger;
import third_party;
import infinity_exception;

namespace infinity {

export u64 ConvertValueToU64(const Value &value);

// used in block_entry and segment_entry
// BinaryFuse: cannot be updated after creation, used in sealed segments
// TODO: add a real-time bloom filter for unsealed segments
export class ProbabilisticDataFilter {
private:
    // should always be resized and initialized when minmax filter is built
    Vector<UniquePtr<BinaryFuse>> binary_fuse_filters_;

public:
    constexpr static std::string_view JsonTag = "probabilistic_data_filter";

    ProbabilisticDataFilter() = default;

    explicit ProbabilisticDataFilter(u32 column_count) : binary_fuse_filters_(column_count) {
        // fill the vector with valid objects
        for (u32 i = 0; i < binary_fuse_filters_.size(); ++i) {
            binary_fuse_filters_[i] = MakeUnique<BinaryFuse>();
        }
    }

    inline bool MayContain(TxnTimeStamp query_ts, ColumnID column_id, const Value &value) const {
        auto key_val = ConvertValueToU64(value);
        return binary_fuse_filters_[column_id]->Contain(query_ts, key_val);
    }

    // finish build in one function
    void Build(TxnTimeStamp begin_ts, ColumnID column_id, u64 *data, u32 count);

    u32 GetSerializeSizeInBytes() const;

    void SerializeToStringStream(OStringStream &os, u32 total_binary_bytes = 0) const;

    void DeserializeFromStringStream(IStringStream &is);

    void SaveToJsonFile(nlohmann::json &entry_json) const;

    bool LoadFromJsonFile(const nlohmann::json &entry_json);
};

export template <typename ValueType>
u64 ConvertValueToU64(const ValueType &value);

template <>
u64 ConvertValueToU64<BooleanT>(const BooleanT &value) {
    return value ? 1 : 0;
}

template <>
u64 ConvertValueToU64<TinyIntT>(const TinyIntT &value) {
    return value;
}

template <>
u64 ConvertValueToU64<SmallIntT>(const SmallIntT &value) {
    return value;
}

template <>
u64 ConvertValueToU64<IntegerT>(const IntegerT &value) {
    return value;
}

template <>
u64 ConvertValueToU64<BigIntT>(const BigIntT &value) {
    return value;
}

template <>
u64 ConvertValueToU64<HugeIntT>(const HugeIntT &value) {
    return value.GetHash();
}

template <>
u64 ConvertValueToU64<DecimalT>(const DecimalT &value) {
    return value.GetHash();
}

template <>
u64 ConvertValueToU64<DateT>(const DateT &value) {
    return value.GetValue();
}

template <>
u64 ConvertValueToU64<TimeT>(const TimeT &value) {
    return value.GetValue();
}

template <>
u64 ConvertValueToU64<DateTimeT>(const DateTimeT &value) {
    return value.GetEpochTime();
}

template <>
u64 ConvertValueToU64<TimestampT>(const TimestampT &value) {
    return value.GetEpochTime();
}

template <>
u64 ConvertValueToU64<String>(const String &value) {
    return std::hash<String>{}(value);
}

} // namespace infinity