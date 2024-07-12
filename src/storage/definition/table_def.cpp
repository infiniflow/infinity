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

#include <sstream>

module table_def;

import stl;
import data_type;
import serialize;
import internal_types;
import infinity_exception;
import parsed_expr;
import constant_expr;
import logger;

namespace infinity {

String TableDef::ToString() const {
    std::stringstream ss;
    ss << *table_name_ << '[';
    for (SizeT i = 0; auto &column_ptr : this->columns_) {
        ss << column_ptr->ToString();
        ++i;
        if (i != this->column_count()) {
            ss << ", ";
        }
    }

    ss << ']';
    return ss.str();
}

void TableDef::UnionWith(const SharedPtr<TableDef> &other) {
    SizeT this_column_count = this->column_count();
    SizeT other_column_count = other->column_count();
    this->columns_.reserve(this_column_count + other_column_count);
    this->columns_.insert(this->columns_.end(), other->columns_.begin(), other->columns_.end());

    SizeT total_column_count = this_column_count + other_column_count;
    for (SizeT idx = this_column_count; idx < total_column_count; ++idx) {
        column_name2id_[columns_[idx]->name()] = idx;
    }
}

bool TableDef::operator==(const TableDef &other) const {
    if (this->schema_name_.get() == nullptr || other.schema_name_.get() == nullptr || this->table_name_.get() == nullptr ||
        other.table_name_.get() == nullptr || !IsEqual(*(this->schema_name_), *(other.schema_name_)) ||
        !IsEqual(*(this->table_name_), *(other.table_name_)) || this->columns_.size() != other.columns_.size() ||
        this->column_name2id_.size() != other.column_name2id_.size()) {
        return false;
    }
    for (u32 i = 0; i < this->columns_.size(); i++) {
        // Compare each column
        const ColumnDef &cd1 = *(this->columns_[i]);
        const ColumnDef &cd2 = *(other.columns_[i]);
        if (cd1 != cd2) {
            return false;
        }
    }
    return true;
}

bool TableDef::operator!=(const TableDef &other) const { return !operator==(other); }

i32 TableDef::GetSizeInBytes() const {
    i32 size = 0;
    size += sizeof(i32) + schema_name_->length();
    size += sizeof(i32) + table_name_->length();
    size += sizeof(i32);
    for (u32 i = 0; i < columns_.size(); i++) {
        const ColumnDef &cd = *columns_[i];
        size += sizeof(i64); // id_
        size += cd.column_type_->GetSizeInBytes();
        size += sizeof(i32) + cd.name_.length();
        size += sizeof(i32);
        size += cd.constraints_.size() * sizeof(ConstraintType);
        size += (dynamic_cast<ConstantExpr *>(cd.default_expr_.get()))->GetSizeInBytes();
        size += sizeof(u8); // build_bloom_filter_
    }
    return size;
}

void TableDef::WriteAdv(char *&ptr) const {
    WriteBufAdv(ptr, *schema_name_);
    WriteBufAdv(ptr, *table_name_);
    WriteBufAdv(ptr, (i32)(columns_.size()));
    SizeT column_count = columns_.size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        const ColumnDef &cd = *columns_[idx];
        WriteBufAdv<i64>(ptr, cd.id_);
        cd.column_type_->WriteAdv(ptr);
        WriteBufAdv(ptr, cd.name_);
        WriteBufAdv(ptr, (i32)cd.constraints_.size());
        for (const auto &cons : cd.constraints_) {
            WriteBufAdv(ptr, cons);
        }
        (dynamic_cast<ConstantExpr *>(cd.default_expr_.get()))->WriteAdv(ptr);
        u8 bf = cd.build_bloom_filter_ ? 1 : 0;
        WriteBufAdv(ptr, bf);
    }
    return;
}

SharedPtr<TableDef> TableDef::ReadAdv(char *&ptr, i32 maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    if (maxbytes <= 0) {
        String error_message = "ptr goes out of range when reading TableDef";
        UnrecoverableError(error_message);
    }
    String schema_name = ReadBufAdv<String>(ptr);
    String table_name = ReadBufAdv<String>(ptr);
    i32 columns_size = ReadBufAdv<i32>(ptr);
    Vector<SharedPtr<ColumnDef>> columns;
    for (i32 i = 0; i < columns_size; i++) {
        i64 id = ReadBufAdv<i64>(ptr);
        maxbytes = ptr_end - ptr;
        if (maxbytes <= 0) {
            String error_message = "ptr goes out of range when reading TableDef";
        UnrecoverableError(error_message);
        }
        SharedPtr<DataType> column_type = DataType::ReadAdv(ptr, maxbytes);
        String column_name = ReadBufAdv<String>(ptr);
        i32 constraints_size = ReadBufAdv<i32>(ptr);
        std::set<ConstraintType> constraints;
        for (i32 j = 0; j < constraints_size; j++) {
            ConstraintType ct = ReadBufAdv<ConstraintType>(ptr);
            constraints.insert(ct);
        }
        SharedPtr<ParsedExpr> default_expr = ConstantExpr::ReadAdv(ptr, maxbytes);
        SharedPtr<ColumnDef> cd = MakeShared<ColumnDef>(id, column_type, column_name, constraints, default_expr);
        u8 bf = ReadBufAdv<u8>(ptr);
        cd->build_bloom_filter_ = bf;
        columns.push_back(cd);
    }
    maxbytes = ptr_end - ptr;
    if (maxbytes < 0) {
        String error_message = "ptr goes out of range when reading TableDef";
        UnrecoverableError(error_message);
    }
    return TableDef::Make(MakeShared<String>(schema_name), MakeShared<String>(table_name), columns);
}

} // namespace infinity