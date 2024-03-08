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
import stl;
import third_party;
module fst;

namespace infinity {
FstError::FstError(FstErrorCode code, const char *msg) : code_(code) { msg_ = MakeUnique<String>(msg); }

FstError::FstError(FstError &s) { MoveError(s); }

FstError::FstError(FstError &&s) noexcept { MoveError(s); }

FstError &FstError::operator=(FstError &s) noexcept {
    MoveError(s);
    return *this;
}

FstError &FstError::operator=(FstError &&s) noexcept {
    MoveError(s);
    return *this;
}

void FstError::MoveError(FstError &s) {
    code_ = s.code_;
    msg_ = std::move(s.msg_);
}

void FstError::MoveError(FstError &&s) {
    code_ = s.code_;
    msg_ = std::move(s.msg_);
    s.msg_ = nullptr;
}

FstError FstError::Version(u64 expected, u64 got) {
    return FstError(FstErrorCode::kVersion, MakeUnique<String>(fmt::format("Version mismatch, expected {}, got {}", expected, got)));
}

FstError FstError::Format(SizeT size) { return FstError(FstErrorCode::kFormat, MakeUnique<String>(fmt::format("Invalid fst size, got {}", size))); }

FstError FstError::ChecksumMismatch(u32 expected, u32 got) {
    return FstError(FstErrorCode::kChecksumMismatch, MakeUnique<String>(fmt::format("Checksum mismatch, expected {}, got {}", expected, got)));
}

FstError FstError::ChecksumMissing() { return FstError(FstErrorCode::kChecksumMissing, MakeUnique<String>("Checksum missing")); }

FstError FstError::DuplicatedKey(u8 *got_data, SizeT got_len) {
    return FstError(FstErrorCode::kDuplicatedKey, MakeUnique<String>(fmt::format("Duplicated key, got {}", FormatBytes(got_data, got_len))));
}

FstError FstError::OutOfOrder(u8 *prev_data, SizeT prev_len, u8 *got_data, SizeT got_len) {
    return FstError(
        FstErrorCode::kOutofOrderKey,
        MakeUnique<String>(fmt::format("Out of order key, prev {}, got {}", FormatBytes(prev_data, prev_len), FormatBytes(got_data, got_len))));
}

FstError FstError::FromUtf8(u8 *got_data, SizeT got_len, SizeT valid_up_to) {
    return FstError(FstErrorCode::kFromUtf8,
                    MakeUnique<String>(fmt::format("From utf8 error, got {}, valid_up_to {}", FormatBytes(got_data, got_len), valid_up_to)));
}

FstError FstError::Unsupported(const String &method) {
    return FstError(FstErrorCode::kUnsupported, MakeUnique<String>(fmt::format("Method {} is unsupported", method)));
}

} // namespace infinity