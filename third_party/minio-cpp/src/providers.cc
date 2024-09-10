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

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include "miniocpp/providers.h"

#include <INIReader.h>

#include <fstream>
#include <iosfwd>
#include <list>
#include <nlohmann/json.hpp>
#include <string>
#include <type_traits>

#include "miniocpp/credentials.h"
#include "miniocpp/error.h"
#include "miniocpp/http.h"
#include "miniocpp/signer.h"
#include "miniocpp/utils.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2def.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#endif

namespace minio::creds {

error::Error checkLoopbackHost(const std::string& host) {
  struct addrinfo hints = {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int status;
  struct addrinfo* res = nullptr;
  if ((status = getaddrinfo(host.c_str(), nullptr, &hints, &res)) != 0) {
    return error::Error(std::string("getaddrinfo: ") + gai_strerror(status));
  }

  for (struct addrinfo* ai = res; ai != nullptr; ai = ai->ai_next) {
    std::string ip(inet_ntoa(((struct sockaddr_in*)ai->ai_addr)->sin_addr));
    if (!utils::StartsWith(ip, "127.")) {
      return error::Error(host + " is not loopback only host");
    }
  }

  freeaddrinfo(res);  // free the linked list

  return error::SUCCESS;
}

Provider::~Provider() {}

ChainedProvider::~ChainedProvider() {}

Credentials ChainedProvider::Fetch() {
  if (err_) return Credentials{err_};

  if (creds_) return creds_;

  if (provider_ != nullptr) {
    creds_ = provider_->Fetch();
    if (creds_) return creds_;
  }

  for (auto provider : providers_) {
    provider_ = provider;
    creds_ = provider_->Fetch();
    if (creds_) return creds_;
  }

  return error::make<Credentials>("All providers fail to fetch credentials");
}

StaticProvider::StaticProvider(std::string access_key, std::string secret_key,
                               std::string session_token) {
  this->creds_ = Credentials(error::SUCCESS, std::move(access_key),
                             std::move(secret_key), std::move(session_token));
}

StaticProvider::~StaticProvider() {}

Credentials StaticProvider::Fetch() { return creds_; }

EnvAwsProvider::EnvAwsProvider() {
  std::string access_key;
  std::string secret_key;
  std::string session_token;

  if (!utils::GetEnv(access_key, "AWS_ACCESS_KEY_ID")) {
    utils::GetEnv(access_key, "AWS_ACCESS_KEY");
  }
  if (!utils::GetEnv(secret_key, "AWS_SECRET_ACCESS_KEY")) {
    utils::GetEnv(secret_key, "AWS_SECRET_KEY");
  }
  utils::GetEnv(session_token, "AWS_SESSION_TOKEN");

  this->creds_ = Credentials(error::SUCCESS, std::move(access_key),
                             std::move(secret_key), std::move(session_token));
}

EnvAwsProvider::~EnvAwsProvider() {}

Credentials EnvAwsProvider::Fetch() { return creds_; }

EnvMinioProvider::EnvMinioProvider() {
  std::string access_key;
  std::string secret_key;

  utils::GetEnv(access_key, "MINIO_ACCESS_KEY");
  utils::GetEnv(secret_key, "MINIO_SECRET_KEY");
  this->creds_ =
      Credentials(error::SUCCESS, std::move(access_key), std::move(secret_key));
}

EnvMinioProvider::~EnvMinioProvider() {}

Credentials EnvMinioProvider::Fetch() { return creds_; }

AwsConfigProvider::AwsConfigProvider(std::string filename,
                                     std::string profile) {
  if (filename.empty()) {
    if (!utils::GetEnv(filename, "AWS_SHARED_CREDENTIALS_FILE")) {
      filename = utils::GetHomeDir() + "/aws/credentials";
    }
  }

  if (profile.empty()) {
    if (!utils::GetEnv(profile, "AWS_PROFILE")) profile = "default";
  }

  INIReader reader(filename);
  if (reader.ParseError() < 0) {
    this->creds_ = error::make<Credentials>("unable to read " + filename);
  } else {
    this->creds_ = Credentials{error::SUCCESS,
                               reader.Get(profile, "aws_access_key_id", ""),
                               reader.Get(profile, "aws_secret_access_key", ""),
                               reader.Get(profile, "aws_session_token", "")};
  }
}

AwsConfigProvider::~AwsConfigProvider() {}

Credentials AwsConfigProvider::Fetch() { return creds_; }

MinioClientConfigProvider::MinioClientConfigProvider(std::string filename,
                                                     std::string alias) {
  if (filename.empty()) filename = utils::GetHomeDir() + "/.mc/config.json";

  if (alias.empty()) {
    if (!utils::GetEnv(alias, "MINIO_ALIAS")) alias = "s3";
  }

  std::ifstream ifs(filename);
  nlohmann::json json = nlohmann::json::parse(ifs);
  ifs.close();

  nlohmann::json aliases;
  if (json.contains("hosts")) {
    aliases = json["hosts"];
  } else if (json.contains("aliases")) {
    aliases = json["aliases"];
  } else {
    this->creds_ =
        error::make<Credentials>("invalid configuration in file " + filename);
    return;
  }

  if (!aliases.contains(alias)) {
    this->creds_ = error::make<Credentials>(
        "alias " + alias + " not found in MinIO client configuration file " +
        filename);
    return;
  }

  this->creds_ = Credentials{error::SUCCESS, aliases[alias]["accessKey"],
                             aliases[alias]["secretKey"]};
}

MinioClientConfigProvider::~MinioClientConfigProvider() {}

Credentials MinioClientConfigProvider::Fetch() { return creds_; }

AssumeRoleProvider::AssumeRoleProvider(
    http::Url sts_endpoint, std::string access_key, std::string secret_key,
    unsigned int duration_seconds, std::string policy, std::string region,
    std::string role_arn, std::string role_session_name,
    std::string external_id) {
  this->sts_endpoint_ = sts_endpoint;
  this->access_key_ = access_key;
  this->secret_key_ = secret_key;
  this->region_ = region;

  if (duration_seconds < DEFAULT_DURATION_SECONDS) {
    duration_seconds = DEFAULT_DURATION_SECONDS;
  }

  utils::Multimap map;
  map.Add("Action", "AssumeRole");
  map.Add("Version", "2011-06-15");
  map.Add("DurationSeconds", std::to_string(duration_seconds));
  if (!role_arn.empty()) map.Add("RoleArn", role_arn);
  if (!role_session_name.empty()) {
    map.Add("RoleSessionName", role_session_name);
  }
  if (!policy.empty()) map.Add("Policy", policy);
  if (!external_id.empty()) map.Add("ExternalId", external_id);

  this->body_ = map.ToQueryString();
  this->content_sha256_ = utils::Sha256Hash(body_);
}

AssumeRoleProvider::~AssumeRoleProvider() {}

Credentials AssumeRoleProvider::Fetch() {
  if (err_) return Credentials{err_};

  if (creds_) return creds_;

  utils::UtcTime date = utils::UtcTime::Now();
  utils::Multimap headers;
  headers.Add("Content-Type", "application/x-www-form-urlencoded");
  headers.Add("Host", sts_endpoint_.host);
  headers.Add("X-Amz-Date", date.ToAmzDate());

  http::Method method = http::Method::kPost;
  signer::SignV4STS(method, sts_endpoint_.path, region_, headers,
                    utils::Multimap(), access_key_, secret_key_,
                    content_sha256_, date);

  http::Request req(method, sts_endpoint_);
  req.headers = headers;
  req.body = body_;
  http::Response resp = req.Execute();
  if (!resp) {
    creds_ = Credentials{resp.Error()};
  } else {
    creds_ = Credentials::ParseXML(resp.body, "AssumeRoleResult");
  }

  return creds_;
}

WebIdentityClientGrantsProvider::WebIdentityClientGrantsProvider(
    JwtFunction jwtfunc, http::Url sts_endpoint, unsigned int duration_seconds,
    std::string policy, std::string role_arn, std::string role_session_name) {
  this->jwtfunc_ = jwtfunc;
  this->sts_endpoint_ = sts_endpoint;
  this->duration_seconds_ = duration_seconds;
  this->policy_ = policy;
  this->role_arn_ = role_arn;
  this->role_session_name_ = role_session_name;
}

WebIdentityClientGrantsProvider::~WebIdentityClientGrantsProvider() {}

unsigned int WebIdentityClientGrantsProvider::getDurationSeconds(
    unsigned int expiry) const {
  if (duration_seconds_) expiry = duration_seconds_;
  if (expiry > MAX_DURATION_SECONDS) return MAX_DURATION_SECONDS;
  if (expiry == 0) return expiry;
  if (expiry < MIN_DURATION_SECONDS) return MIN_DURATION_SECONDS;
  return expiry;
}

Credentials WebIdentityClientGrantsProvider::Fetch() {
  if (creds_) return creds_;

  Jwt jwt = jwtfunc_();

  utils::Multimap map;
  map.Add("Version", "2011-06-15");
  unsigned int duration_seconds = getDurationSeconds(jwt.expiry);
  if (duration_seconds) {
    map.Add("DurationSeconds", std::to_string(duration_seconds));
  }
  if (!policy_.empty()) map.Add("Policy", policy_);

  if (IsWebIdentity()) {
    map.Add("Action", "AssumeRoleWithWebIdentity");
    map.Add("WebIdentityToken", jwt.token);
    if (!role_arn_.empty()) {
      map.Add("RoleArn", role_arn_);
      if (!role_session_name_.empty()) {
        map.Add("RoleSessionName", role_session_name_);
      } else {
        map.Add("RoleSessionName", utils::UtcTime::Now().ToISO8601UTC());
      }
    }
  } else {
    map.Add("Action", "AssumeRoleWithClientGrants");
    map.Add("Token", jwt.token);
  }

  http::Url url = sts_endpoint_;
  url.query_string = map.ToQueryString();
  http::Request req(http::Method::kPost, url);
  http::Response resp = req.Execute();
  if (!resp) {
    creds_ = Credentials{resp.Error()};
  } else {
    creds_ = Credentials::ParseXML(
        resp.body, IsWebIdentity() ? "AssumeRoleWithWebIdentityResult"
                                   : "AssumeRoleWithClientGrantsResult");
  }
  return creds_;
}

ClientGrantsProvider::ClientGrantsProvider(
    JwtFunction jwtfunc, http::Url sts_endpoint, unsigned int duration_seconds,
    std::string policy, std::string role_arn, std::string role_session_name)
    : WebIdentityClientGrantsProvider(jwtfunc, sts_endpoint, duration_seconds,
                                      policy, role_arn, role_session_name) {}

ClientGrantsProvider::~ClientGrantsProvider() {}

bool ClientGrantsProvider::IsWebIdentity() const { return false; }

WebIdentityProvider::WebIdentityProvider(
    JwtFunction jwtfunc, http::Url sts_endpoint, unsigned int duration_seconds,
    std::string policy, std::string role_arn, std::string role_session_name)
    : WebIdentityClientGrantsProvider(jwtfunc, sts_endpoint, duration_seconds,
                                      policy, role_arn, role_session_name) {}

WebIdentityProvider::~WebIdentityProvider() {}

bool WebIdentityProvider::IsWebIdentity() const { return true; }

IamAwsProvider::IamAwsProvider(http::Url custom_endpoint) {
  this->custom_endpoint_ = custom_endpoint;
  utils::GetEnv(this->token_file_, "AWS_WEB_IDENTITY_TOKEN_FILE");
  utils::GetEnv(this->aws_region_, "AWS_REGION");
  utils::GetEnv(this->role_arn_, "AWS_ROLE_ARN");
  utils::GetEnv(this->role_session_name_, "AWS_ROLE_SESSION_NAME");
  utils::GetEnv(this->relative_uri_, "AWS_CONTAINER_CREDENTIALS_RELATIVE_URI");
  if (!this->relative_uri_.empty() && this->relative_uri_.front() != '/') {
    this->relative_uri_ = "/" + this->relative_uri_;
  }
  utils::GetEnv(this->full_uri_, "AWS_CONTAINER_CREDENTIALS_FULL_URI");
}

IamAwsProvider::~IamAwsProvider() {}

Credentials IamAwsProvider::Fetch() {
  if (creds_) return creds_;

  http::Url url = custom_endpoint_;
  if (!token_file_.empty()) {
    if (!url) {
      url.https = true;
      url.host = "sts.amazonaws.com";
      if (!aws_region_.empty()) {
        url.host = "sts." + aws_region_ + ".amazonaws.com";
      }
    }

    WebIdentityProvider provider = WebIdentityProvider(
        [&token_file = token_file_]() -> Jwt {
          std::ifstream ifs(token_file);
          nlohmann::json json = nlohmann::json::parse(ifs);
          ifs.close();
          return Jwt(std::move(json["access_token"]), json["expires_in"]);
        },
        url, 0, "", role_arn_, role_session_name_);
    creds_ = provider.Fetch();
    return creds_;
  }

  if (!relative_uri_.empty()) {
    if (!url) {
      url.https = true;
      url.host = "169.254.170.2";
      url.path = relative_uri_;
    }
  } else if (!full_uri_.empty()) {
    if (!url) url = http::Url::Parse(full_uri_);
    if (error::Error err = checkLoopbackHost(url.host)) {
      creds_ = Credentials{err};
      return creds_;
    }
  } else {
    if (!url) {
      url.https = true;
      url.host = "169.254.169.254";
      url.path = "/latest/meta-data/iam/security-credentials/";
    }

    std::string role_name;
    if (error::Error err = getRoleName(role_name, url)) {
      creds_ = Credentials{err};
      return creds_;
    }

    url.path += "/" + role_name;
  }

  creds_ = fetch(url);
  return creds_;
}

Credentials IamAwsProvider::fetch(http::Url url) {
  http::Request req(http::Method::kGet, url);
  http::Response resp = req.Execute();
  if (!resp) return Credentials{resp.Error()};

  nlohmann::json json = nlohmann::json::parse(resp.body);
  std::string code = json.value("Code", "Success");
  if (code != "Success") {
    return error::make<Credentials>(url.String() + " failed with code " + code +
                                    " and message " +
                                    json.value("Message", ""));
  }

  std::string expiration = json["Expiration"];
  return Credentials{error::SUCCESS, json["AccessKeyId"],
                     json["SecretAccessKey"], json["Token"],
                     utils::UtcTime::FromISO8601UTC(expiration.c_str())};
}

error::Error IamAwsProvider::getRoleName(std::string& role_name,
                                         http::Url url) const {
  http::Request req(http::Method::kGet, url);
  http::Response resp = req.Execute();
  if (!resp) return resp.Error();

  std::list<std::string> role_names;
  std::string lines = resp.body;
  size_t pos;
  while ((pos = lines.find("\n")) != std::string::npos) {
    role_names.push_back(lines.substr(0, pos));
    lines.erase(0, pos + 1);
  }
  if (!lines.empty()) role_names.push_back(lines);

  if (role_names.empty()) {
    return error::Error(std::string("no IAM roles attached to EC2 service ") +
                        url.String());
  }

  role_name = utils::Trim(role_names.front(), '\r');
  return error::SUCCESS;
}

LdapIdentityProvider::LdapIdentityProvider(http::Url sts_endpoint,
                                           std::string ldap_username,
                                           std::string ldap_password) {
  this->sts_endpoint_ = sts_endpoint;
  utils::Multimap map;
  map.Add("Action", "AssumeRoleWithLDAPIdentity");
  map.Add("Version", "2011-06-15");
  map.Add("LDAPUsername", ldap_username);
  map.Add("LDAPPassword", ldap_password);
  this->sts_endpoint_.query_string = map.ToQueryString();
}

LdapIdentityProvider::~LdapIdentityProvider() {}

Credentials LdapIdentityProvider::Fetch() {
  if (creds_) return creds_;

  http::Request req(http::Method::kPost, sts_endpoint_);
  http::Response resp = req.Execute();
  if (!resp) return Credentials{resp.Error()};

  creds_ = Credentials::ParseXML(resp.body, "AssumeRoleWithLDAPIdentityResult");
  return creds_;
}

CertificateIdentityProvider::CertificateIdentityProvider(
    http::Url sts_endpoint, std::string key_file, std::string cert_file,
    std::string ssl_cert_file, unsigned int duration_seconds) {
  if (!sts_endpoint.https) {
    this->err_ = error::Error("sts endpoint scheme must be HTTPS");
    return;
  }

  if (key_file.empty() || cert_file.empty()) {
    this->err_ = error::Error("client key and certificate must be provided");
    return;
  }

  unsigned int expiry = duration_seconds;
  if (duration_seconds < DEFAULT_DURATION_SECONDS) {
    expiry = DEFAULT_DURATION_SECONDS;
  }

  utils::Multimap map;
  map.Add("Action", "AssumeRoleWithCertificate");
  map.Add("Version", "2011-06-15");
  map.Add("DurationSeconds", std::to_string(expiry));

  sts_endpoint_ = sts_endpoint;
  sts_endpoint_.query_string = map.ToQueryString();
  key_file_ = key_file;
  cert_file_ = cert_file;
  ssl_cert_file_ = ssl_cert_file;
}

CertificateIdentityProvider::~CertificateIdentityProvider() {}

Credentials CertificateIdentityProvider::Fetch() {
  if (err_) return Credentials{err_};

  if (creds_) return creds_;

  http::Request req(http::Method::kPost, sts_endpoint_);
  req.ssl_cert_file = ssl_cert_file_;
  req.key_file = key_file_;
  req.cert_file = cert_file_;

  http::Response resp = req.Execute();
  if (!resp) return Credentials{resp.Error()};

  creds_ = Credentials::ParseXML(resp.body, "AssumeRoleWithCertificateResult");
  return creds_;
}

}  // namespace minio::creds
