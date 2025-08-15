// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

export module infinity_core:utility;

import :infinity_type;

import std;
import std.compat;

export namespace infinity::Utility {

size_t NextPowerOfTwo(size_t input);
std::string FormatByteSize(u64 byte_size);
std::string FormatTimeInfo(u64 seconds);

} // namespace infinity::Utility

export namespace infinity {

enum class IdentifierValidationStatus {
    kOk,
    kEmpty,
    kExceedLimit,
    kInvalidName,
};

IdentifierValidationStatus IdentifierValidation(const std::string &identifier);

bool ParseIPPort(const std::string &str, std::string &ip, i64 &port);

std::string StringTransform(const std::string &source, const std::string &from, const std::string &to);

std::string CalcMD5(const char *input_str, size_t length);

std::string CalcMD5(const std::string &filename);

std::tuple<u64, bool> ExtractU64FromStringSuffix(const std::string &src, size_t offset);

std::vector<std::string> Partition(const std::string &text, char delimiter);

std::string Concat(const std::vector<std::string> &v, char delimiter);

void ToLower(std::string &str) {
    std::ranges::transform(str, str.begin(), [](auto c) { return std::tolower(c); });
}

void ToUpper(std::string &str) {
    std::ranges::transform(str, str.begin(), [](auto c) { return std::toupper(c); });
}

std::vector<std::string> SplitStrByComma(std::string str);

template <typename T1, typename T2>
struct CompareByFirst {
    using P = std::pair<T1, T2>;

    bool operator()(const P &lhs, const P &rhs) const { return lhs.first < rhs.first; }
};

template <typename T1, typename T2>
struct CompareByFirstReverse {
    using P = std::pair<T1, T2>;

    bool operator()(const P &lhs, const P &rhs) const { return lhs.first > rhs.first; }
};

std::string TrimPath(const std::string &path) {
    const auto pos = path.find("/src/");
    if (pos == std::string::npos)
        return path;
    return path.substr(pos + 1);
}

} // namespace infinity
