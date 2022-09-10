//
// Created by JinHai on 2022/9/9.
//

#include "vector_operations.h"
#include "common/utility/infinity_assert.h"

namespace infinity {


template<typename SType, typename DType>
struct VectorCopy {
    static void Loop(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data) {
        for(size_t i = source_start; i < source_start + length; ++ i) {
            target_data.push_back(static_cast<DType>(std::any_cast<SType>(source_data[i])));
        }
    }
};

template<typename SType>
struct VectorCopy<SType, std::string> {
    static void Loop(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data) {
        for(size_t i = source_start; i < source_start + length; ++ i) {
            auto s = std::any_cast<SType>(source_data[i]);
            target_data.push_back(std::to_string(s));
        }
    }
};

template<>
struct VectorCopy<bool, std::string> {
    static void Loop(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data) {
        for(size_t i = source_start; i < source_start + length; ++ i) {
            if(std::any_cast<bool>(source_data[i])) {
                target_data.emplace_back(std::string("true"));
            } else {
                target_data.emplace_back(std::string("false"));
            }
        }
    }
};

template<typename SType>
struct VectorCopy<SType, DecimalT> {
    static void Loop(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data) {}
};

template<typename DType>
struct VectorCopy<std::string, DType> {
    static void Loop(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data) {}
};

template<typename DType>
struct VectorCopy<DecimalT, DType> {
    static void Loop(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data) {}
};

template<typename SType>
void
VectorCopyFrom(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data, LogicalTypeId target_type) {
    switch(target_type) {
        case LogicalTypeId::kBoolean: return VectorCopy<SType, BooleanT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kTinyInt: return VectorCopy<SType, TinyIntT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kSmallInt: return VectorCopy<SType, SmallIntT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kInteger: return VectorCopy<SType, IntegerT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kBigInt: return VectorCopy<SType, BigIntT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kFloat: return VectorCopy<SType, FloatT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kDouble: return VectorCopy<SType, DoubleT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kDecimal: {
            NotImplementError("Decimal type can't be cast now.");
//            return VectorCopy<SType, DecimalT>::Loop(source_data, target_data);
        }
        case LogicalTypeId::kDate: return VectorCopy<SType, DateT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kTime: return VectorCopy<SType, TimeT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kDateTime: return VectorCopy<SType, DateTimeT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kInterval: return VectorCopy<SType, IntervalT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kVarchar: return VectorCopy<SType, VarcharT>::Loop(source_data, source_start, length, target_data);
        case LogicalTypeId::kText: return VectorCopy<SType, TextT>::Loop(source_data, source_start, length, target_data);
        default:
            TypeError("Invalid target data type");
    }
}

template<typename DType>
void
VectorFromStrToNumeric(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data) {
    for(size_t i = source_start; i < source_start + length; ++ i) {
        target_data.push_back(static_cast<DType>(std::stoll(std::any_cast<std::string>(source_data[i]))));
    }
}

void
VectorFromStrToBool(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data) {
    for(size_t i = source_start; i < source_start + length; ++ i) {
        auto str = std::any_cast<std::string>(source_data[i]);
        std::transform(str.begin(),str.end(), str.begin(), toupper);
        if(str == "TRUE") {
            target_data.emplace_back(true);
        } else {
            target_data.emplace_back(false);
        }
    }
}

void
VectorCopyFromStr(const std::vector<std::any>& source_data, int64_t source_start, int64_t length, std::vector<std::any>& target_data, LogicalTypeId target_type) {
    switch(target_type) {
        case LogicalTypeId::kBoolean: return VectorFromStrToBool(source_data, source_start, length, target_data);
        case LogicalTypeId::kTinyInt: return VectorFromStrToNumeric<TinyIntT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kSmallInt: return VectorFromStrToNumeric<SmallIntT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kInteger: return VectorFromStrToNumeric<IntegerT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kBigInt: return VectorFromStrToNumeric<BigIntT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kFloat: return VectorFromStrToNumeric<FloatT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kDouble: return VectorFromStrToNumeric<DoubleT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kDecimal: {
            NotImplementError("Decimal type can't be cast now.");
        }
        // FIXME: String to Date/Time/Datetime/Interval need to be handle specially.
        case LogicalTypeId::kDate: return VectorFromStrToNumeric<DateT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kTime: return VectorFromStrToNumeric<TimeT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kDateTime: return VectorFromStrToNumeric<DateTimeT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kInterval: return VectorFromStrToNumeric<IntervalT>(source_data, source_start, length, target_data);
        case LogicalTypeId::kVarchar:
        case LogicalTypeId::kText: target_data = source_data;
        default:
            TypeError("Invalid target data type");
    }
}

void
VectorOperation::VectorCast(const Chunk& source, int64_t source_start, int64_t length, Chunk& target) {

    // Check if the type are same, if so just use the source chunk;
    if(source.DataType() == target.DataType()) {
        if(source_start == 0 && target.row_count() == 0) {
            target = source;
        } else {
            for(int64_t i = source_start; i < source_start + length; ++ i) {
                target.data().emplace_back(source.data()[i]);
            }
        }

        return ;
    }

    int64_t row_count = source.row_count();
    // Prepare the target chunk memory space
    target.data().reserve(row_count);

    switch(source.DataType().GetTypeId()) {
        case LogicalTypeId::kBoolean:
            VectorCopyFrom<BooleanT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kTinyInt:
            VectorCopyFrom<TinyIntT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kSmallInt:
            VectorCopyFrom<SmallIntT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kInteger:
            VectorCopyFrom<IntegerT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kBigInt:
            VectorCopyFrom<BigIntT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kFloat:
            VectorCopyFrom<FloatT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kDouble:
            VectorCopyFrom<DoubleT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kDecimal:
            NotImplementError("Decimal type can't be cast now.");
        case LogicalTypeId::kDate:
            VectorCopyFrom<DateT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kTime:
            VectorCopyFrom<TimeT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kDateTime:
            VectorCopyFrom<DateTimeT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kInterval:
            VectorCopyFrom<IntervalT>(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
            break;
        case LogicalTypeId::kVarchar:
        case LogicalTypeId::kText:
            VectorCopyFromStr(source.data(), source_start, length, target.data(), target.DataType().GetTypeId());
//            VectorCopyFrom<TextT>(source.data(), target.data(), row_count, target.DataType().GetTypeId());
            break;
        default:
            TypeError("Invalid source data type");
    }
}

void
VectorOperation::VectorCast(const Chunk& source, Chunk& target) {
    if(source.DataType() == target.DataType()) {
        target = source;
        return ;
    }

    int64_t row_count = source.row_count();
    return VectorCast(source, 0, row_count, target);
}

}
