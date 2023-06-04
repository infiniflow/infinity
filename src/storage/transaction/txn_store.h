//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "storage/meta/entry/db_entry.h"

namespace infinity {

class TxnStore {
public:
    explicit
    TxnStore();

    SharedPtr<DBEntry>
    CreateDatabase(const String& name);
};

}
