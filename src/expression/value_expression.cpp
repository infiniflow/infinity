//
// Created by JinHai on 2022/8/5.
//

#include "value_expression.h"
#include "common/utility/infinity_assert.h"

#include <utility>
#include <sstream>

namespace infinity {

template<typename Type>
static std::string
GenerateTitle(const LogicalType& data_type_, const std::any& value) {
    return data_type_.ToString() + "_" + std::to_string(std::any_cast<Type>(value));
}

template<>
std::string
GenerateTitle<std::string>(const LogicalType& data_type, const std::any& value) {
    return data_type.ToString() + "_" + std::any_cast<std::string>(value);
}

ValueExpression::ValueExpression(LogicalType data_type, std::any value)
    : BaseExpression(ExpressionType::kValue, {}), data_type_(data_type), value_(std::move(value)) {}

ValueExpression::ValueExpression(LogicalType data_type)
    : BaseExpression(ExpressionType::kValue, {}), data_type_(data_type), value_(std::any()) {}

std::string
ValueExpression::ToString() const {
    std::stringstream ss;

    switch (data_type_.GetTypeId()) {
        case LogicalTypeId::kBoolean: return GenerateTitle<bool>(data_type_, value_);
        case LogicalTypeId::kTinyInt: return GenerateTitle<int8_t>(data_type_, value_);
        case LogicalTypeId::kSmallInt: return GenerateTitle<int16_t>(data_type_, value_);
        case LogicalTypeId::kInteger: return GenerateTitle<int32_t>(data_type_, value_);
        case LogicalTypeId::kBigInt: return GenerateTitle<int64_t>(data_type_, value_);
        case LogicalTypeId::kFloat: return GenerateTitle<float>(data_type_, value_);
        case LogicalTypeId::kDouble: return GenerateTitle<double>(data_type_, value_);
//        case LogicalTypeId::kDecimal:
//            break;
//        case LogicalTypeId::kDate:
//            break;
//        case LogicalTypeId::kTime:
//            break;
//        case LogicalTypeId::kDateTime:
//            break;
//        case LogicalTypeId::kInterval:
//            break;
        case LogicalTypeId::kVarchar: return GenerateTitle<std::string>(data_type_, value_);
        case LogicalTypeId::kText: return GenerateTitle<std::string>(data_type_, value_);
        default:
            PlannerError("Unsupported value type.");
    }
}

void
ValueExpression::AppendToChunk(Chunk& chunk) {
    chunk.Append(value_);
}

}
