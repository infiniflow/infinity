// MinIO C++ Library for Amazon S3 Compatible Cloud Storage
// Copyright 2022-2024 MinIO, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MINIO_CPP_SELECT_H_INCLUDED
#define MINIO_CPP_SELECT_H_INCLUDED

#include <map>
#include <string>
#include <type_traits>

#include "error.h"
#include "http.h"
#include "types.h"

namespace minio::s3 {

class SelectHandler {
 private:
  SelectResultFunction result_func_ = nullptr;

  bool done_ = false;
  std::string response_;

  std::string prelude_;
  bool prelude_read_ = false;

  std::string prelude_crc_;
  bool prelude_crc_read_ = false;

  size_t total_length_ = 0;

  std::string data_;
  bool data_read_ = false;

  std::string message_crc_;
  bool message_crc_read_ = false;

  void Reset();
  bool ReadPrelude();
  bool ReadPreludeCrc();
  bool ReadData();
  bool ReadMessageCrc();
  error::Error DecodeHeader(std::map<std::string, std::string>& headers,
                            std::string data);
  bool process(const http::DataFunctionArgs& args, bool& cont);

 public:
  explicit SelectHandler(SelectResultFunction result_func)
      : result_func_(std::move(result_func)) {}

  ~SelectHandler() = default;

  bool DataFunction(const http::DataFunctionArgs& args);
};  // struct SelectHandler

}  // namespace minio::s3

#endif  // MINIO_CPP_SELECT_H_INCLUDED
