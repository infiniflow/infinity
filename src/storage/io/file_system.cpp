//
// Created by jinhai on 23-5-16.
//

module;

import stl;

module file_system;

namespace infinity {

i64 FileHandler::Read(void *data, u64 nbytes) { return file_system_.Read(*this, data, nbytes); }

i64 FileHandler::Write(void *data, u64 nbytes) { return file_system_.Write(*this, data, nbytes); }

void FileHandler::Sync() { return file_system_.SyncFile(*this); }

void FileHandler::Close() { return file_system_.Close(*this); }

} // namespace infinity
