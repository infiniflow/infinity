module;

export module column_inverter;
import stl;
import column_vector;
import parser;

namespace infinity {
export class ColumnInverter {
public:
    virtual void InvertColumn(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids) = 0;

    virtual void InvertColumn(u32 doc_id, const String &val) = 0;

    virtual void Commit() = 0;

    virtual void Flush() = 0;
};
} // namespace infinity