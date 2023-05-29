//
// Created by jinhai on 23-5-27.
//

#include "database.h"
#include "database_helper.h"

#include <unistd.h>

namespace infinity {

Database::~Database() {
    if(locker_fd_ != 0) {
        // Unlock the locker file
        close(locker_fd_);
    }
}

SharedPtr<Database>
Database::Open(const infinity::String &dir_name) {
    SharedPtr<Database> database_ptr = MakeShared<Database>();
    database_ptr->locker_fd_ =  DatabaseHelper::CreateLockerFile(dir_name);

    return database_ptr;
}

}