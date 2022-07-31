//
// Created by JinHai on 2022/7/31.
//

#include "block.h"
#include "common/utility/asserter.h"

namespace infinity {

Block::Block(const std::shared_ptr<TableDefinition>& table_def, TableType type, uint64_t limit)
    : type_(type), block_limit_(limit), column_defs_(table_def->columns()) {
    for(const auto& column_def: table_def->columns()) {
        auto logical_type = column_def.logical_type();
        switch(logical_type.GetTypeId()) {
            case LogicalTypeId::kBoolean: {
                columns_.emplace_back(std::make_shared<Chunk<BooleanT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kTinyInt: {
                columns_.emplace_back(std::make_shared<Chunk<TinyIntT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kSmallInt: {
                columns_.emplace_back(std::make_shared<Chunk<SmallIntT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kInteger: {
                columns_.emplace_back(std::make_shared<Chunk<IntegerT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kBigInt: {
                columns_.emplace_back(std::make_shared<Chunk<BigIntT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kFloat: {
                columns_.emplace_back(std::make_shared<Chunk<FloatT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kDouble: {
                columns_.emplace_back(std::make_shared<Chunk<DoubleT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kDecimal: {
                columns_.emplace_back(std::make_shared<Chunk<DecimalT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kDate: {
                columns_.emplace_back(std::make_shared<Chunk<DateT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kTime: {
                columns_.emplace_back(std::make_shared<Chunk<TimeT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kDateTime: {
                columns_.emplace_back(std::make_shared<Chunk<DateTimeT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kInterval: {
                columns_.emplace_back(std::make_shared<Chunk<IntervalT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kVarchar: {
                columns_.emplace_back(std::make_shared<Chunk<VarcharT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            case LogicalTypeId::kText: {
                columns_.emplace_back(std::make_shared<Chunk<TextT>>(TableType::kFixedRowCount, logical_type, limit));
                break;
            }
            default:
                ResponseError("Wrong column type");
        }
    }
}


void Block::Append(const std::vector<std::any>& chunks, uint64_t start_idx) {
    uint64_t column_count = columns_.size();
    Assert(chunks.size() == column_count, "Wrong column data are appended to the row group");

    for(uint64_t i = 0; i < column_count; ++ i) {
        switch(column_defs_[i].logical_type().GetTypeId()) {
            case LogicalTypeId::kBoolean: {
                auto column = std::any_cast<std::shared_ptr<Chunk<BooleanT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<BooleanT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kTinyInt: {
                auto column = std::any_cast<std::shared_ptr<Chunk<TinyIntT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<TinyIntT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kSmallInt: {
                auto column = std::any_cast<std::shared_ptr<Chunk<SmallIntT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<SmallIntT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kInteger: {
                auto column = std::any_cast<std::shared_ptr<Chunk<IntegerT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<IntegerT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kBigInt: {
                auto column = std::any_cast<std::shared_ptr<Chunk<BigIntT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<BigIntT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kFloat: {
                auto column = std::any_cast<std::shared_ptr<Chunk<FloatT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<FloatT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kDouble: {
                auto column = std::any_cast<std::shared_ptr<Chunk<DoubleT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<DoubleT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kDecimal: {
                auto column = std::any_cast<std::shared_ptr<Chunk<DecimalT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<DecimalT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kDate: {
                auto column = std::any_cast<std::shared_ptr<Chunk<DateT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<DateT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kTime: {
                auto column = std::any_cast<std::shared_ptr<Chunk<TimeT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<TimeT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kDateTime: {
                auto column = std::any_cast<std::shared_ptr<Chunk<DateTimeT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<DateTimeT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kInterval: {
                auto column = std::any_cast<std::shared_ptr<Chunk<IntervalT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<IntervalT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kVarchar: {
                auto column = std::any_cast<std::shared_ptr<Chunk<VarcharT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<VarcharT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            case LogicalTypeId::kText: {
                auto column = std::any_cast<std::shared_ptr<Chunk<TextT>>>(columns_[i]);
                auto chunk = std::any_cast<std::shared_ptr<Chunk<TextT>>>(chunks[i]);
                column->Append(*chunk, start_idx);
                break;
            }
            default:
                ResponseError("Wrong column type");
        }
    }
}
}