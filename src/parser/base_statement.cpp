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

#include "base_statement.h"
#include <string>

namespace infinity {

std::string StatementType2Str(StatementType statement_type) {
    switch(statement_type) {
        case StatementType::kInvalidStmt: {
            ParserError("Unexpected error.");
            break;
        }
        case StatementType::kSelect: {
            return "SELECT";
        }
        case StatementType::kCopy: {
            return "COPY";
        }
        case StatementType::kInsert: {
            return "INSERT";
        }
        case StatementType::kUpdate: {
            return "UPDATE";
        }
        case StatementType::kDelete: {
            return "DELETE";
        }
        case StatementType::kCreate: {
            return "CREATE";
        }
        case StatementType::kDrop: {
            return "DROP";
        }
        case StatementType::kPrepare: {
            return "PREPARE";
        }
        case StatementType::kExecute: {
            return "EXECUTE";
        }
        case StatementType::kAlter: {
            return "ALTER";
        }
        case StatementType::kShow: {
            return "SHOW";
        }
        case StatementType::kExplain: {
            return "EXPLAIN";
        }
        case StatementType::kFlush: {
            return "FLUSH";
        }
        case StatementType::kOptimize: {
            return "OPTIMIZE";
        }
        case StatementType::kCommand: {
            return "COMMAND";
        }
        case StatementType::kCompact: {
            return "COMPACT";
        }
        case StatementType::kAdmin: {
            return "ADMIN";
        }
    }

    return "Error";
}

}
