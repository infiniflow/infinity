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

#include "cast_expr.h"

namespace infinity {

CastExpr::~CastExpr() {
    if (expr_ != nullptr) {
        delete expr_;
        expr_ = nullptr;
    }
}

std::string CastExpr::ToString() const {
    std::stringstream ss;
    ss << "CAST(" << expr_->ToString() << "  AS " << data_type_.ToString() << ")";
    return ss.str();
}

} // namespace infinity
