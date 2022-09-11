//
// Created by JinHai on 2022/7/31.
//

#include "chunk.h"
#include "common/utility/infinity_assert.h"

#include <sstream>

static void
VectorBoolToString(const std::vector<std::any>& bool_array, std::stringstream& ss) {
//    size_t row_count = data.size();
    for(std::any data: bool_array) {
        if(std::any_cast<bool>(data)) {
            ss << "true" << std::endl;
        } else {
            ss << "false" << std::endl;
        }
    }
}

template<typename T>
static void
VectorNumericToString(const std::vector<std::any>& numeric_array, std::stringstream& ss) {
    for(std::any data: numeric_array) {
        ss << static_cast<int64_t>(std::any_cast<T>(data)) << std::endl;
    }
}

static void
VectorToString(const std::vector<std::any>& numeric_array, std::stringstream& ss) {
    for(std::any data: numeric_array) {
        ss << std::any_cast<std::string>(data) << std::endl;
    }
}

namespace infinity {
// TODO: unit test !!!
void
Chunk::Append(const Chunk& chunk, int64_t start_idx) {
    StorageAssert(data_type_ == chunk.data_type_,
                  "Trying to append different data from: " + chunk.data_type_.ToString() + " to: " + data_type_.ToString());
    int64_t available_size = limit_ - row_count_;
    size_t insert_size = chunk.data_.size() - start_idx;
    if(available_size >= insert_size) {
        data_.insert(data_.begin() + row_count_, chunk.data_.begin() + start_idx, chunk.data_.end());
    } else {
        data_.insert(data_.begin() + row_count_, chunk.data_.begin() + start_idx, chunk.data_.begin() + available_size);
    }
}

void
Chunk::Append(const std::any& value) {
    data_.emplace_back(value);
    ++ row_count_;
}

std::string
Chunk::ToString() {
    std::stringstream ss;
    switch(data_type_.GetTypeId()) {
        case LogicalTypeId::kBoolean:
            VectorBoolToString(data_, ss);
            break;
        case LogicalTypeId::kTinyInt:
            VectorNumericToString<TinyIntT>(data_, ss);
            break;
        case LogicalTypeId::kSmallInt:
            VectorNumericToString<SmallIntT>(data_, ss);
            break;
        case LogicalTypeId::kInteger:
            VectorNumericToString<IntegerT>(data_, ss);
            break;
        case LogicalTypeId::kBigInt:
            VectorNumericToString<BigIntT>(data_, ss);
            break;
        case LogicalTypeId::kFloat:
            VectorNumericToString<FloatT>(data_, ss);
            break;
        case LogicalTypeId::kDouble:
            VectorNumericToString<DoubleT>(data_, ss);
            break;
        case LogicalTypeId::kDecimal:
            TypeError("Decimal is not supported now.");
            break;
            // FIXME: Date/Time/DateTime/Interval need to be handled specially.
        case LogicalTypeId::kDate:
            VectorNumericToString<DateT>(data_, ss);
            break;
        case LogicalTypeId::kTime:
            VectorNumericToString<TimeT>(data_, ss);
            break;
        case LogicalTypeId::kDateTime:
            VectorNumericToString<DateTimeT>(data_, ss);
            break;
        case LogicalTypeId::kInterval:
            VectorNumericToString<IntervalT>(data_, ss);
            break;
        case LogicalTypeId::kVarchar:
            VectorToString(data_, ss);
            break;
        case LogicalTypeId::kText:
            VectorToString(data_, ss);
            break;
        default:
            TypeError("Unsupported type");
    }
    return ss.str();
}

std::string
Chunk::RowToString(int64_t row_id) {
    switch(data_type_.GetTypeId()) {
        case LogicalTypeId::kBoolean: {
            if(std::any_cast<bool>(data_[row_id])) {
                return "true";
            } else {
                return "false";
            }
        }
        case LogicalTypeId::kTinyInt: {
            int8_t value = std::any_cast<int8_t>(data_[row_id]);
            return std::to_string(value);
            break;
        }

        case LogicalTypeId::kSmallInt: {
            int16_t value = std::any_cast<int16_t>(data_[row_id]);
            return std::to_string(value);
            break;
        }
        case LogicalTypeId::kInteger: {
            int32_t value = std::any_cast<int32_t>(data_[row_id]);
            return std::to_string(value);
            break;
        }
        case LogicalTypeId::kBigInt: {
            int64_t value = std::any_cast<int64_t>(data_[row_id]);
            return std::to_string(value);
            break;
        }
        case LogicalTypeId::kFloat: {
            float value = std::any_cast<float>(data_[row_id]);
            return std::to_string(value);
            break;
        }
        case LogicalTypeId::kDouble: {
            double value = std::any_cast<double>(data_[row_id]);
            return std::to_string(value);
            break;
        }
        case LogicalTypeId::kDecimal: {
            TypeError("Decimal is not supported now.");
            break;
        }
        case LogicalTypeId::kDate: {
            int32_t value = std::any_cast<int32_t>(data_[row_id]);
            return std::to_string(value);
            break;
        }
        case LogicalTypeId::kTime: {
            int32_t value = std::any_cast<int32_t>(data_[row_id]);
            return std::to_string(value);
            break;
        }
        case LogicalTypeId::kDateTime: {
            int64_t value = std::any_cast<int64_t>(data_[row_id]);
            return std::to_string(value);
            break;
        }
        case LogicalTypeId::kInterval: {
            int64_t value = std::any_cast<int64_t>(data_[row_id]);
            return std::to_string(value);
            break;
        }
        case LogicalTypeId::kVarchar:
            return std::any_cast<std::string>(data_[row_id]);
        case LogicalTypeId::kText:
            return std::any_cast<std::string>(data_[row_id]);
        default:
            TypeError("Unsupported type");
    }
}

}