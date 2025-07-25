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

export module infinity_core:pg_message;

import :stl;

export namespace infinity {

// Each message has the length field to indicate the message size.
constexpr auto LENGTH_FIELD_SIZE = 4u;

constexpr char NULL_END = '\0';

enum class NullTerminator : bool {
    kYes = true,
    kNo = false,
};

// From https://www.postgresql.org/docs/14/protocol-message-formats.html
enum class PGMessageType : unsigned char {
    // Response
    kAuthentication = 'R',
    kError = 'E',
    kParameterStatus = 'S',
    kReadyForQuery = 'Z',
    kRowDescription = 'T',
    kData = 'D',
    kComplete = 'C',

    // Errors
    kHumanReadableError = 'M',

    // SSL related
    kSSLYes = 'S',
    kSSLNo = 'N',

    // Commands
    kExecuteCommand = 'E',
    kSyncCommand = 'S',
    kFlushCommand = 'H',
    kTerminateCommand = 'X',
    kDescribeCommand = 'D',
    kBindCommand = 'B',
    kParseCommand = 'P',
    kSimpleQueryCommand = 'Q',
    kCloseCommand = 'C',
};

enum class TransactionStateType : unsigned char {
    kIDLE = 'I',  // Not in a transaction block
    kBlock = 'T', // In a transaction block
    kFail = 'E',  // In a failed transaction
};

} // namespace infinity
