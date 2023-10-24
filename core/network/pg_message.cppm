//
// Created by JinHai on 2022/7/20.
//

module;

import stl;

export module pg_message;

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
