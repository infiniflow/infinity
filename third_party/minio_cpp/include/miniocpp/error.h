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

#ifndef MINIO_CPP_ERROR_H_INCLUDED
#define MINIO_CPP_ERROR_H_INCLUDED

#include <ostream>
#include <string>
#include <type_traits>

namespace minio::error {

class Error {
 private:
  std::string msg_;

 public:
  Error() = default;

  explicit Error(std::string msg) : msg_(std::move(msg)) {}

  Error(const Error&) = default;
  Error(Error&& v) = default;

  Error& operator=(const Error&) = default;
  Error& operator=(Error&& v) = default;

  ~Error() = default;

  const std::string& String() const { return msg_; }
  explicit operator bool() const { return !msg_.empty(); }

  friend std::ostream& operator<<(std::ostream& s, const Error& e) {
    return s << e.msg_;
  }
};  // class Error

extern const Error SUCCESS;

template <typename T_RESULT, typename... TA>
inline T_RESULT make(TA&&... args) {
  return T_RESULT{Error(std::forward<TA>(args)...)};
}

}  // namespace minio::error

#endif  // MINIO_CPP_ERROR_H_INCLUDED
