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

#ifndef MINIO_CPP_PROVIDERS_H_INCLUDED
#define MINIO_CPP_PROVIDERS_H_INCLUDED

#include <functional>
#include <list>
#include <string>
#include <type_traits>

#include "credentials.h"
#include "error.h"
#include "http.h"

namespace minio::creds {

// 1 day.
static constexpr unsigned DEFAULT_DURATION_SECONDS = 60 * 60 * 24;

// 15 minutes.
static constexpr unsigned MIN_DURATION_SECONDS = 60 * 15;

// 7 days.
static constexpr unsigned MAX_DURATION_SECONDS = 60 * 60 * 24 * 7;

struct Jwt {
  std::string token;
  unsigned int expiry = 0;

  Jwt() = default;
  explicit Jwt(std::string token, unsigned int expiry)
      : token(std::move(token)), expiry(expiry) {}
  ~Jwt() = default;

  explicit operator bool() const { return !token.empty(); }
};  // struct Jwt

using JwtFunction = std::function<Jwt()>;

error::Error checkLoopbackHost(const std::string& host);

/**
 * Credential provider interface.
 */
class Provider {
 protected:
  error::Error err_;
  Credentials creds_;

 public:
  Provider() = default;
  virtual ~Provider();

  explicit operator bool() const { return !err_; }

  virtual Credentials Fetch() = 0;
};  // class Provider

class ChainedProvider : public Provider {
 private:
  std::list<Provider*> providers_;
  Provider* provider_ = nullptr;

 public:
  explicit ChainedProvider(std::list<Provider*> providers)
      : providers_(std::move(providers)) {}

  virtual ~ChainedProvider();

  virtual Credentials Fetch() override;
};  // class ChainedProvider

/**
 * Static credential provider.
 */
class StaticProvider : public Provider {
 public:
  StaticProvider(std::string access_key, std::string secret_key,
                 std::string session_token = {});
  virtual ~StaticProvider();

  virtual Credentials Fetch() override;
};  // class StaticProvider

class EnvAwsProvider : public Provider {
 public:
  EnvAwsProvider();
  virtual ~EnvAwsProvider();

  virtual Credentials Fetch() override;
};  // class EnvAwsProvider

class EnvMinioProvider : public Provider {
 public:
  EnvMinioProvider();
  virtual ~EnvMinioProvider();

  virtual Credentials Fetch() override;
};  // class EnvMinioProvider

class AwsConfigProvider : public Provider {
 public:
  explicit AwsConfigProvider(std::string filename = {},
                             std::string profile = {});
  virtual ~AwsConfigProvider();

  virtual Credentials Fetch() override;
};  // class AwsConfigProvider

class MinioClientConfigProvider : public Provider {
 public:
  explicit MinioClientConfigProvider(std::string filename = {},
                                     std::string alias = {});
  virtual ~MinioClientConfigProvider();

  virtual Credentials Fetch() override;
};  // class MinioClientConfigProvider

class AssumeRoleProvider : public Provider {
 private:
  http::Url sts_endpoint_;
  std::string access_key_;
  std::string secret_key_;
  std::string region_;
  std::string body_;
  std::string content_sha256_;

 public:
  AssumeRoleProvider(http::Url sts_endpoint, std::string access_key,
                     std::string secret_key, unsigned int duration_seconds = 0,
                     std::string policy = {}, std::string region = {},
                     std::string role_arn = {},
                     std::string role_session_name = {},
                     std::string external_id = {});

  virtual ~AssumeRoleProvider();

  virtual Credentials Fetch() override;
};  // class AssumeRoleProvider

class WebIdentityClientGrantsProvider : public Provider {
 private:
  JwtFunction jwtfunc_ = nullptr;
  http::Url sts_endpoint_;
  unsigned int duration_seconds_ = 0;
  std::string policy_;
  std::string role_arn_;
  std::string role_session_name_;

 public:
  WebIdentityClientGrantsProvider(JwtFunction jwtfunc, http::Url sts_endpoint,
                                  unsigned int duration_seconds = 0,
                                  std::string policy = {},
                                  std::string role_arn = {},
                                  std::string role_session_name = {});

  virtual ~WebIdentityClientGrantsProvider();

  virtual bool IsWebIdentity() const = 0;

  unsigned int getDurationSeconds(unsigned int expiry) const;

  virtual Credentials Fetch() override;
};  // class WebIdentityClientGrantsProvider

class ClientGrantsProvider : public WebIdentityClientGrantsProvider {
 public:
  ClientGrantsProvider(JwtFunction jwtfunc, http::Url sts_endpoint,
                       unsigned int duration_seconds = 0,
                       std::string policy = {}, std::string role_arn = {},
                       std::string role_session_name = {});

  virtual ~ClientGrantsProvider();

  virtual bool IsWebIdentity() const override;
};  // class ClientGrantsProvider

class WebIdentityProvider : public WebIdentityClientGrantsProvider {
 public:
  WebIdentityProvider(JwtFunction jwtfunc, http::Url sts_endpoint,
                      unsigned int duration_seconds = 0,
                      std::string policy = {}, std::string role_arn = {},
                      std::string role_session_name = {});

  virtual ~WebIdentityProvider();

  virtual bool IsWebIdentity() const override;
};  // class WebIdentityProvider

class IamAwsProvider : public Provider {
 private:
  http::Url custom_endpoint_;
  std::string token_file_;
  std::string aws_region_;
  std::string role_arn_;
  std::string role_session_name_;
  std::string relative_uri_;
  std::string full_uri_;

 public:
  explicit IamAwsProvider(http::Url custom_endpoint = http::Url());
  virtual ~IamAwsProvider();

  virtual Credentials Fetch() override;

 private:
  Credentials fetch(http::Url url);
  error::Error getRoleName(std::string& role_name, http::Url url) const;
};  // class IamAwsProvider

class LdapIdentityProvider : public Provider {
 private:
  http::Url sts_endpoint_;

 public:
  LdapIdentityProvider(http::Url sts_endpoint, std::string ldap_username,
                       std::string ldap_password);

  virtual ~LdapIdentityProvider();

  virtual Credentials Fetch() override;
};  // class LdapIdentityProvider

struct CertificateIdentityProvider : public Provider {
 private:
  http::Url sts_endpoint_;
  std::string key_file_;
  std::string cert_file_;
  std::string ssl_cert_file_;

 public:
  CertificateIdentityProvider(http::Url sts_endpoint, std::string key_file,
                              std::string cert_file,
                              std::string ssl_cert_file = {},
                              unsigned int duration_seconds = 0);

  virtual ~CertificateIdentityProvider();

  virtual Credentials Fetch() override;
};  // struct CertificateIdentityProvider

}  // namespace minio::creds

#endif  // MINIO_CPP_PROVIDERS_H_INCLUDED
