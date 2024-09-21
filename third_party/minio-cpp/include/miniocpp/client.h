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

#ifndef MINIO_CPP_CLIENT_H_INCLUDED
#define MINIO_CPP_CLIENT_H_INCLUDED

#include <list>
#include <string>

#include "args.h"
#include "baseclient.h"
#include "error.h"
#include "providers.h"
#include "request.h"
#include "response.h"

namespace minio::s3 {

class Client;

class ListObjectsResult {
 private:
  Client* client_ = nullptr;
  ListObjectsArgs args_;
  bool failed_ = false;
  ListObjectsResponse resp_;
  std::list<Item>::iterator itr_;

  void Populate();

 public:
  explicit ListObjectsResult(error::Error err);
  ListObjectsResult(Client* const client, const ListObjectsArgs& args);
  ListObjectsResult(Client* const client, ListObjectsArgs&& args);
  ~ListObjectsResult() = default;

  Item& operator*() const { return *itr_; }
  explicit operator bool() const { return itr_ != resp_.contents.end(); }

  ListObjectsResult& operator++() {
    itr_++;
    if (!failed_ && itr_ == resp_.contents.end() && resp_.is_truncated) {
      Populate();
    }
    return *this;
  }

  ListObjectsResult operator++(int) {
    ListObjectsResult curr = *this;
    ++(*this);
    return curr;
  }
};  // class ListObjectsResult

class RemoveObjectsResult {
 private:
  Client* client_ = nullptr;
  RemoveObjectsArgs args_;
  bool done_ = false;
  RemoveObjectsResponse resp_;
  std::list<DeleteError>::iterator itr_;

  void Populate();

 public:
  explicit RemoveObjectsResult(error::Error err);
  RemoveObjectsResult(Client* const client, const RemoveObjectsArgs& args);
  RemoveObjectsResult(Client* const client, RemoveObjectsArgs&& args);
  ~RemoveObjectsResult() = default;

  DeleteError& operator*() const { return *itr_; }
  explicit operator bool() const { return itr_ != resp_.errors.end(); }
  RemoveObjectsResult& operator++() {
    itr_++;
    if (!done_ && itr_ == resp_.errors.end()) {
      Populate();
    }
    return *this;
  }
  RemoveObjectsResult operator++(int) {
    RemoveObjectsResult curr = *this;
    ++(*this);
    return curr;
  }
};  // class RemoveObjectsResult

/**
 * Simple Storage Service (aka S3) client to perform bucket and object
 * operations.
 */
class Client : public BaseClient {
 protected:
  StatObjectResponse CalculatePartCount(size_t& part_count,
                                        std::list<ComposeSource> sources);
  ComposeObjectResponse ComposeObject(ComposeObjectArgs args,
                                      std::string& upload_id);
  PutObjectResponse PutObject(PutObjectArgs args, std::string& upload_id,
                              char* buf);

 public:
  explicit Client(BaseUrl& base_url, creds::Provider* const provider = nullptr);
  ~Client() = default;

  ComposeObjectResponse ComposeObject(ComposeObjectArgs args);
  CopyObjectResponse CopyObject(CopyObjectArgs args);
  DownloadObjectResponse DownloadObject(DownloadObjectArgs args);
  ListObjectsResult ListObjects(ListObjectsArgs args);
  PutObjectResponse PutObject(PutObjectArgs args);
  UploadObjectResponse UploadObject(UploadObjectArgs args);
  RemoveObjectsResult RemoveObjects(RemoveObjectsArgs args);
};  // class Client

}  // namespace minio::s3

#endif  // MINIO_CPP_CLIENT_H_INCLUDED
