// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:regex_index_rewrite;

import :base_expression;
import :base_table_ref;

import std;

namespace infinity {

class QueryContext;

// Makes `regex(column, '<pattern>')` use a full-text index.
//
// A regular expression cannot be answered by an index directly, but every match
// must contain the literal runs the pattern proves mandatory. Those runs are
// turned into `filter_fulltext(column, '<run>', 'operator=and;similarity=boolean')`
// predicates, which the existing push-down rewrites into an index scan over the
// grams of each run. The original `regex(...)` call is kept in the conjunction,
// so it survives as the residual filter and verifies every candidate. The
// narrowing only ever removes rows the pattern cannot match, which makes the
// rewrite transparent to the result.
//
// Nothing happens when the column has no full-text index, when its analyzer is
// not a sparse gram analyzer, or when the pattern has no usable literal run:
// the query then keeps the plain scan it would have had.
export class RegexIndexRewrite {
public:
    static std::shared_ptr<BaseExpression>
    Rewrite(QueryContext *query_context, const BaseTableRef *base_table_ref, const std::shared_ptr<BaseExpression> &expression);
};

} // namespace infinity
