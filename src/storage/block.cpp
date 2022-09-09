//
// Created by JinHai on 2022/7/31.
//

#include "block.h"
#include "common/utility/infinity_assert.h"

#include <sstream>

namespace infinity {

Block::Block(const std::shared_ptr<TableDefinition>& table_def, TableType type, int64_t limit)
    : type_(type), block_limit_(limit), column_defs_(table_def->columns()) {
    for(const auto& column_def: table_def->columns()) {
        auto logical_type = column_def.logical_type();
        columns_.emplace_back(Chunk(logical_type, limit));
    }
}

void
Block::Append(const std::vector<Chunk>& input_chunks, int64_t start_idx)
{
    uint64_t column_count = columns_.size();
    StorageAssert(input_chunks.size() == column_count, "Wrong column data are appended to the row group");

    for(uint64_t i = 0; i < column_count; ++ i) {
        // TODO: check the chunk type, if not try to cast it.
        if(columns_[i].DataType() != input_chunks[i].DataType()) {

        } else {
            columns_[i].Append(input_chunks[i], start_idx);
        }
    }

    ++ row_count_;
}

std::string
Block::AsStringRow() {
    std::stringstream ss;

    uint64_t column_count = column_defs_.size();

    // Iterator all row of the blocks
    for(int64_t row_id = 0; row_id < row_count_; ++ row_id) {

        // Iterate all column of this block
        for(uint64_t column_id = 0; column_id < column_count; ++ column_id) {

            // Get the chunk type
            const LogicalType& chunk_type = column_defs_[column_id].logical_type();

            Chunk& chunk = columns_[column_id];
            const std::any& data = chunk.data()[row_id];
            switch(chunk_type.GetTypeId()) {
                case LogicalTypeId::kBoolean: {
                    auto value = std::any_cast<BooleanT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kTinyInt: {
                    auto value = std::any_cast<TinyIntT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kSmallInt: {
                    auto value = std::any_cast<SmallIntT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kInteger: {
                    auto value = std::any_cast<IntegerT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kBigInt: {
                    auto value = std::any_cast<BigIntT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kFloat: {
                    auto value = std::any_cast<FloatT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kDouble: {
                    auto value = std::any_cast<DoubleT>(data);
                    break;
                }
                case LogicalTypeId::kDecimal: {
                    auto value = std::any_cast<DecimalT>(data);

                    // TODO: Decimal type to string
                    ss << "Decimal Value" << " ";
                    break;
                }
                case LogicalTypeId::kDate: {
                    auto value = std::any_cast<DateT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kTime: {
                    auto value = std::any_cast<TimeT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kDateTime: {
                    auto value = std::any_cast<DateTimeT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kInterval: {
                    auto value = std::any_cast<IntervalT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kVarchar: {
                    auto value = std::any_cast<VarcharT>(data);
                    ss << value << " ";
                    break;
                }
                case LogicalTypeId::kText: {
                    auto value = std::any_cast<TextT>(data);
                    ss << value << " ";
                    break;
                }
                default: {
                    StorageError("Wrong column type");
                }
            }
        }

        ss << std::endl;
    }

    return ss.str();
}
}