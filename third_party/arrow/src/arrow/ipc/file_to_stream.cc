// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <iostream>
#include <memory>
#include <string>

#include "arrow/io/file.h"
#include "arrow/io/stdio.h"
#include "arrow/ipc/reader.h"
#include "arrow/ipc/writer.h"
#include "arrow/result.h"
#include "arrow/status.h"

namespace arrow {

class RecordBatch;

namespace ipc {

// Reads a file on the file system and prints to stdout the stream version of it.
Status ConvertToStream(const char* path) {
  io::StdoutStream sink;

  ARROW_ASSIGN_OR_RAISE(auto in_file, io::ReadableFile::Open(path));
  ARROW_ASSIGN_OR_RAISE(auto reader, ipc::RecordBatchFileReader::Open(in_file.get()));
  ARROW_ASSIGN_OR_RAISE(auto writer, ipc::MakeStreamWriter(&sink, reader->schema(),
                                                           IpcWriteOptions::Defaults()));
  for (int i = 0; i < reader->num_record_batches(); ++i) {
    ARROW_ASSIGN_OR_RAISE(std::shared_ptr<RecordBatch> chunk, reader->ReadRecordBatch(i));
    RETURN_NOT_OK(writer->WriteRecordBatch(*chunk));
  }
  return writer->Close();
}

}  // namespace ipc
}  // namespace arrow

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: file-to-stream <input arrow file>" << std::endl;
    return 1;
  }
  arrow::Status status = arrow::ipc::ConvertToStream(argv[1]);
  if (!status.ok()) {
    std::cerr << "Could not convert to stream: " << status.ToString() << std::endl;
    return 1;
  }
  return 0;
}
