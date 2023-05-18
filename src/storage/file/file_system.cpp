//
// Created by jinhai on 23-5-16.
//

#include "file_system.h"

namespace infinity {

i64
FileHandler::Read(void *data, infinity::u64 nbytes) {
    return file_system_.Read(*this, data, nbytes);
}

i64
FileHandler::Write(void* data, u64 nbytes) {
    return file_system_.Write(*this, data, nbytes);
}

void
FileHandler::Sync() {
    return file_system_.SyncFile(*this);
}

}


