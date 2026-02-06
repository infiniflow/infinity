//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

export module infinity_core:filter_expression_push_down;

import :base_expression;
import :base_table_ref;
import :index_base;
import :fast_rough_filter;
import :roaring_bitmap;
import :search_driver;

namespace infinity {

class NewTxn;
class QueryContext;
class LogicalNode;

// TODO: equivalent expression rewrite optimization

// TODO: now do not support "not" expression in index scan

// 1. secondary index
// 2. filter_fulltext
// 3. AND, OR
export struct IndexFilterEvaluator {
    enum class Type {
        kAllTrue,
        kAllFalse,
        kSecondaryIndex,
        kFulltextIndex,
        kAnd,
        kOr,
        kInvalid,
    };

    Type type() const { return type_; }
    virtual ~IndexFilterEvaluator() = default;
    [[nodiscard]] virtual Bitmask Evaluate(SegmentID segment_id, SegmentOffset segment_row_count) const = 0;

protected:
    Type type_;
    explicit IndexFilterEvaluator(const Type type) : type_(type) {}
};

export struct IndexScanFilterExpressionPushDownResult {
    std::shared_ptr<BaseExpression> index_filter_;
    std::shared_ptr<BaseExpression> leftover_filter_;

    std::unique_ptr<IndexFilterEvaluator> index_filter_evaluator_;
};

// Cache key for LogicalMatch nodes.
struct MatchCacheKey {
    std::string fields_;
    std::string matching_text_;
    std::string default_field_;
    FulltextQueryOperatorOption query_operator_option_;

    MatchCacheKey(std::string fields, std::string text, std::string default_field, FulltextQueryOperatorOption query_operator_option)
        : fields_(std::move(fields)), matching_text_(std::move(text)), default_field_(std::move(default_field)),
          query_operator_option_(query_operator_option) {}

    bool operator==(const MatchCacheKey &other) const {
        return fields_ == other.fields_ && matching_text_ == other.matching_text_ && default_field_ == other.default_field_ &&
               query_operator_option_ == other.query_operator_option_;
    }
};

// Hash function for MatchCacheKey.
struct MatchCacheKeyHash {
    std::size_t operator()(const MatchCacheKey &key) const noexcept {
        std::size_t h1 = hash_str(key.fields_);
        std::size_t h2 = hash_str(key.matching_text_);
        std::size_t h3 = hash_str(key.default_field_);
        std::size_t h4 = hash_int(static_cast<int>(key.query_operator_option_));
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }

private:
    std::hash<std::string> hash_str;
    std::hash<int> hash_int;
};

export class FilterExpressionPushDown {
public:
    using MatchQueryCache = std::unordered_map<MatchCacheKey, std::shared_ptr<LogicalNode>, MatchCacheKeyHash>;

    static IndexScanFilterExpressionPushDownResult PushDownToIndexScan(QueryContext *query_context,
                                                                       const BaseTableRef *base_table_ref_ptr,
                                                                       const std::shared_ptr<BaseExpression> &expression,
                                                                       const MatchQueryCache *match_cache = nullptr);

    static void BuildFilterFulltextExpression(QueryContext *query_context,
                                              const BaseTableRef *base_table_ref_ptr,
                                              const std::vector<std::shared_ptr<BaseExpression>> &expressions,
                                              const MatchQueryCache *match_cache = nullptr);

    static std::unique_ptr<FastRoughFilterEvaluator> PushDownToFastRoughFilter(std::shared_ptr<BaseExpression> &expression, TableMeta *table_meta);
};

} // namespace infinity