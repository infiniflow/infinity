//
// Created by jinhai on 23-10-16.
//

module;

import base_table;
import stl;

export module table;

namespace infinity {

enum class TableType {
    kInvalid,
    kDataTable,
    kIntermediate,
    kOrderBy,
    kGroupBy,
    kAggregate,
    kCrossProduct,
    kResult,
};

export class Table : public BaseTable {
#if 0
public:
    static SharedPtr<Table> Make(SharedPtr<TableDef> table_def_ptr, TableType type);

    static SharedPtr<Table> MakeResultTable(const Vector<SharedPtr<ColumnDef>> &column_defs);

    static SharedPtr<Table> MakeEmptyResultTable();

public:
    explicit Table(SharedPtr<TableDef> table_def_ptr, TableType type);

public:
    [[nodiscard]] SizeT ColumnCount() const;

    [[nodiscard]] SharedPtr<String> TableName() const;

    [[nodiscard]] const SharedPtr<String> &SchemaName() const;

    SizeT GetColumnIdByName(const String &column_name);

    [[nodiscard]] SizeT row_count() const { return row_count_; }

    [[nodiscard]] TableType type() const { return type_; }

    [[nodiscard]] SizeT DataBlockCount() const { return data_blocks_.size(); }

    [[nodiscard]] SharedPtr<DataBlock> &GetDataBlockById(SizeT idx) {
        StorageAssert(idx < data_blocks_.size(),
                      "Attempt to access invalid index: " + std::to_string(idx) + "/" + std::to_string(DataBlockCount())) return data_blocks_[idx];
    }

    [[nodiscard]] String &GetColumnNameById(SizeT idx) const;

    [[nodiscard]] SharedPtr<DataType> GetColumnTypeById(SizeT idx) const;

    void Append(const SharedPtr<DataBlock> &data_block);

    inline void UpdateRowCount(SizeT row_count) { row_count_ += row_count; }

    inline void SetResultMsg(UniquePtr<String> result_msg) { result_msg_ = std::move(result_msg); }

    [[nodiscard]] inline String *result_msg() const { return result_msg_.get(); }

public:
    [[nodiscard]] String ToString() const;

    [[nodiscard]] SharedPtr<Vector<RowID>> GetRowIDVector() const;

    // Currently this method is used in aggregate operator.
    void UnionWith(const SharedPtr<Table> &other);

public:
    SharedPtr<TableDef> definition_ptr_{};
    SizeT row_count_{0};
    TableType type_{TableType::kInvalid};
    Vector<SharedPtr<DataBlock>> data_blocks_{};
    SharedPtr<String> result_msg_{};
#endif
};

} // namespace infinity

