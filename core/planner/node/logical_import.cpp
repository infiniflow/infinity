//
// Created by jinhai on 23-8-27.
//

module;

import stl;
import column_binding;
import parser;

module logical_import;

namespace infinity {

Vector<ColumnBinding> LogicalImport::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalImport::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalImport::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalImport::ToString(i64 &space) const {
//    std::stringstream ss;
//    String arrow_str;
//    if (space > 3) {
//        space -= 4;
//        arrow_str = "->  ";
//    }
//
//    ss << String(space, ' ') << "-> "
//       << "Import from: " << file_path_;
//
//    switch (file_type_) {
//        case CopyFileType::kCSV: {
//            ss << "(CSV) ";
//            break;
//        }
//        case CopyFileType::kJSON: {
//            ss << "(JSON) ";
//            break;
//        }
//        case CopyFileType::kFVECS: {
//            ss << "(FVECS) ";
//            break;
//        }
//    }
//
//    DBEntry *db_entry = TableCollectionMeta::GetDBEntry(table_collection_entry_->table_collection_meta_);
//    ss << "to " << *db_entry->db_name_ << '.' << *table_collection_entry_->table_collection_name_;
//
//    space += arrow_str.size();
//
//    return ss.str();
}

} // namespace infinity
