//
// Created by jinhai on 22-12-25.
//

module;

import std;
import stl;
import parser;
import serialize;

import infinity_exception;

module table_def;

namespace infinity {

String TableDef::ToString() const {
    StringStream ss;
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
    for (i32 i = 0; i < this->columns_.size(); i++) {
        // Compare each column
        const ColumnDef &cd1 = *(this->columns_[i]);
        const ColumnDef &cd2 = *(other.columns_[i]);
        if (cd1.type_ != cd2.type_ || cd1.id_ != cd2.id_ || cd1.column_type_.get() == nullptr || cd2.column_type_.get() == nullptr ||
            *(cd1.column_type_) != *(cd2.column_type_) || !IsEqual(cd1.name_, cd2.name_) || cd1.constraints_.size() != cd2.constraints_.size())
            return false;
        // Convert HashSet to Set
        Set<ConstraintType> constraints1, constraints2;
        for (auto &cons : cd1.constraints_) {
            constraints1.insert(cons);
        }
        for (auto &cons : cd2.constraints_) {
            constraints2.insert(cons);
        }
        Set<ConstraintType>::iterator it1 = constraints1.begin();
        Set<ConstraintType>::iterator it2 = constraints2.begin();
        for (i32 j = 0; j < constraints1.size(); j++) {
            if (*it1 != *it2) {
                return false;
            }
            it1++;
            it2++;
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
    for (i32 i = 0; i < columns_.size(); i++) {
        const ColumnDef &cd = *columns_[i];
        size += sizeof(i64); // id_
        size += cd.column_type_->GetSizeInBytes();
        size += sizeof(i32) + cd.name_.length();
        size += sizeof(i32);
        for (const auto &cons : cd.constraints_) {
            size += sizeof(ConstraintType);
        }
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
    }
    return;
}

SharedPtr<TableDef> TableDef::ReadAdv(char *&ptr, i32 maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    Assert<StorageException>(maxbytes > 0, "ptr goes out of range when reading TableDef");
    String schema_name = ReadBufAdv<String>(ptr);
    String table_name = ReadBufAdv<String>(ptr);
    i32 columns_size = ReadBufAdv<i32>(ptr);
    Vector<SharedPtr<ColumnDef>> columns;
    for (i32 i = 0; i < columns_size; i++) {
        i64 id = ReadBufAdv<i64>(ptr);
        maxbytes = ptr_end - ptr;
        Assert<StorageException>(maxbytes > 0, "ptr goes out of range when reading TableDef");
        SharedPtr<DataType> column_type = DataType::ReadAdv(ptr, maxbytes);
        String column_name = ReadBufAdv<String>(ptr);
        i32 constraints_size = ReadBufAdv<i32>(ptr);
        HashSet<ConstraintType> constraints;
        for (i32 j = 0; j < constraints_size; j++) {
            ConstraintType ct = ReadBufAdv<ConstraintType>(ptr);
            constraints.insert(ct);
        }
        SharedPtr<ColumnDef> cd = MakeShared<ColumnDef>(id, column_type, column_name, constraints);
        columns.push_back(cd);
    }
    maxbytes = ptr_end - ptr;
    Assert<StorageException>(maxbytes >= 0, "ptr goes out of range when reading TableDef");
    return TableDef::Make(MakeShared<String>(schema_name), MakeShared<String>(table_name), columns);
}

} // namespace infinity