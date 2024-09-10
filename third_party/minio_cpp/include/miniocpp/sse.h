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

#ifndef MINIO_CPP_SSE_H_INCLUDED
#define MINIO_CPP_SSE_H_INCLUDED

#include <string>

#include "utils.h"

namespace minio::s3 {

class Sse {
 protected:
  utils::Multimap headers_;
  utils::Multimap copy_headers_;

 public:
  Sse();
  virtual ~Sse();

  utils::Multimap Headers() const;
  utils::Multimap CopyHeaders() const;

  virtual bool TlsRequired() const = 0;
};  // class Sse

class SseCustomerKey : public Sse {
 public:
  explicit SseCustomerKey(std::string_view key);
  virtual ~SseCustomerKey();

  virtual bool TlsRequired() const override;
};  // class SseCustomerKey

class SseKms : public Sse {
 public:
  SseKms(std::string_view key, std::string_view context);
  virtual ~SseKms();

  virtual bool TlsRequired() const override;
};  // class SseKms

class SseS3 : public Sse {
 public:
  SseS3();
  virtual ~SseS3();

  virtual bool TlsRequired() const override;
};  // class SseS3

}  // namespace minio::s3

#endif  // MINIO_CPP_SSE_H_INCLUDED
