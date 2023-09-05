//
// Created by jinhai on 23-9-3.
//

#pragma once

#include <utility>

#include "db_entry.h"
#include "common/types/internal_types.h"
#include "parser/statement/extra/create_view_info.h"

// FIXME: Not finished

namespace infinity {

class ViewMeta;
class BufferManager;
class Txn;

struct ViewEntry : public BaseEntry {
public:
    explicit
    ViewEntry(const SharedPtr<String>& base_dir,
              SharedPtr<CreateViewInfo> create_view_info,
              SharedPtr<String> view_name,
              SharedPtr<Vector<SharedPtr<DataType>>> column_types,
              SharedPtr<Vector<String>> column_names,
              ViewMeta *table_collection_meta,
              u64 txn_id,
              TxnTimeStamp begin_ts)
            : BaseEntry(EntryType::kView),
              base_dir_(base_dir),
              create_view_info_(create_view_info),
              view_name_(std::move(view_name)),
              column_names_(std::move(column_names)),
              column_types_(std::move(column_types)),
              table_collection_meta_(table_collection_meta) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

    [[nodiscard]] inline SelectStatement*
    GetSQLStatement() const {
        return create_view_info_->select_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>&
    column_types() {
        return column_types_;
    }

    inline SharedPtr<Vector<String>>&
    column_names() {
        return column_names_;
    }

    inline const SharedPtr<String>&
    view_name() const {
        return view_name_;
    }

private:
    RWMutex rw_locker_{};

    SharedPtr<CreateViewInfo> create_view_info_;

    SharedPtr<String> base_dir_{};

    SharedPtr<String> view_name_{};

    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    SharedPtr<Vector<String>> column_names_{};

    ViewMeta* table_collection_meta_{};
};

}
