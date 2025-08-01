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

module;

export module infinity_core:utility;

import :stl;

export namespace infinity::Utility {

SizeT NextPowerOfTwo(SizeT input);
String FormatByteSize(u64 byte_size);
String FormatTimeInfo(u64 seconds);

} // namespace infinity::Utility

export namespace infinity {

enum class IdentifierValidationStatus {
    kOk,
    kEmpty,
    kExceedLimit,
    kInvalidName,
};

IdentifierValidationStatus IdentifierValidation(const String &identifier);

bool ParseIPPort(const String &str, String &ip, i64 &port);

String StringTransform(const String &source, const String &from, const String &to);

String CalcMD5(const char *input_str, SizeT length);

String CalcMD5(const String &filename);

Tuple<u64, bool> ExtractU64FromStringSuffix(const String &src, SizeT offset);

Vector<String> Partition(const String &text, char delimiter);

String Concat(const Vector<String> &v, char delimiter);

} // namespace infinity
