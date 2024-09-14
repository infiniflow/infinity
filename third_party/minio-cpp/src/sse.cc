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

#include "miniocpp/sse.h"

#include <string>

#include "miniocpp/utils.h"

namespace minio::s3 {

Sse::Sse() {}

Sse::~Sse() {}

utils::Multimap Sse::Headers() const { return headers_; }

utils::Multimap Sse::CopyHeaders() const { return copy_headers_; }

SseCustomerKey::SseCustomerKey(std::string_view key) {
  std::string b64key = utils::Base64Encode(key);
  std::string md5key = utils::Md5sumHash(key);

  headers_.Add("X-Amz-Server-Side-Encryption-Customer-Algorithm", "AES256");
  headers_.Add("X-Amz-Server-Side-Encryption-Customer-Key", b64key);
  headers_.Add("X-Amz-Server-Side-Encryption-Customer-Key-MD5", md5key);

  copy_headers_.Add(
      "X-Amz-Copy-Source-Server-Side-Encryption-Customer-Algorithm", "AES256");
  copy_headers_.Add("X-Amz-Copy-Source-Server-Side-Encryption-Customer-Key",
                    b64key);
  copy_headers_.Add("X-Amz-Copy-Source-Server-Side-Encryption-Customer-Key-MD5",
                    md5key);
}

SseCustomerKey::~SseCustomerKey() {}

bool SseCustomerKey::TlsRequired() const { return true; }

SseKms::SseKms(std::string_view key, std::string_view context) {
  headers_.Add("X-Amz-Server-Side-Encryption-Aws-Kms-Key-Id", std::string(key));
  headers_.Add("X-Amz-Server-Side-Encryption", "aws:kms");
  if (!context.empty()) {
    headers_.Add("X-Amz-Server-Side-Encryption-Context",
                 utils::Base64Encode(context));
  }
}

SseKms::~SseKms() {}

bool SseKms::TlsRequired() const { return true; }

SseS3::SseS3() { headers_.Add("X-Amz-Server-Side-Encryption", "AES256"); }

SseS3::~SseS3() {}

bool SseS3::TlsRequired() const { return false; }

}  // namespace minio::s3