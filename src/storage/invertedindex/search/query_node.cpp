#include "query_node.h"
#include <cassert>
#include <chrono>

import stl;
import third_party;
import status;
import table_entry;
import column_index_reader;
import posting_iterator;
import index_defines;
import column_length_io;
import infinity_exception;
import logger;

import doc_iterator;
import and_iterator;
import and_not_iterator;
import or_iterator;
import term_doc_iterator;
import phrase_doc_iterator;
import blockmax_wand_iterator;
import blockmax_maxscore_iterator;
import minimum_should_match_iterator;

namespace infinity {

// optimize: from leaf to root, replace tree node in place

// expected property of optimized node:
// 1. children of "not" can only be term, "and" or "and_not", because "not" is not allowed, and "or" will be flattened to not list
// 2. children of "and" can only be term or "or", because "and", "not", "and_not" will be optimized
// 3. children of "or" can only be term, "and" or "and_not", because "or" will be optimized, and "not" is either optimized or not allowed
// 4. "and_not" does not exist in parser output, it is generated during optimization
//    "and_not": first child can be term, "and", "or", other children form a list of "not"
void QueryNode::FilterOptimizeQueryTree() {
    String error_message = "Should not reach here!";
    UnrecoverableError(error_message);
}

std::unique_ptr<QueryNode> QueryNode::GetOptimizedQueryTree(std::unique_ptr<QueryNode> root) {
    if (root->GetType() == QueryNodeType::FILTER) {
        root->FilterOptimizeQueryTree();
        return root;
    }
    auto start_time = std::chrono::high_resolution_clock::now();
    std::unique_ptr<QueryNode> optimized_root;
    if (!root) {
        Status status = Status::SyntaxError("Invalid query statement: Empty query tree");
        RecoverableError(status);
    }
    // push down the weight to the leaf term node
    root->PushDownWeight();
    // optimize the query tree
    switch (root->GetType()) {
        case QueryNodeType::TERM: {
            // no need to optimize
            optimized_root = std::move(root);
            break;
        }
        case QueryNodeType::PHRASE: {
            // no need to optimize
            optimized_root = std::move(root);
            break;
        }
        case QueryNodeType::NOT: {
            Status status = Status::SyntaxError("Invalid query statement: NotQueryNode should not be on the top level");
            RecoverableError(status);
            break;
        }
        case QueryNodeType::AND:
        case QueryNodeType::OR: {
            optimized_root = static_cast<MultiQueryNode *>(root.get())->GetNewOptimizedQueryTree();
            if (optimized_root->GetType() == QueryNodeType::NOT) {
                Status status = Status::SyntaxError("Invalid query statement: NotQueryNode should not be on the top level");
                RecoverableError(status);
            }
            break;
        }
        case QueryNodeType::AND_NOT: {
            String error_message = "Unexpected AndNotQueryNode from parser output";
            UnrecoverableError(error_message);
            break;
        }
        default: {
            String error_message = "GetOptimizedQueryTree: Unexpected case!";
            UnrecoverableError(error_message);
            break;
        }
    }
    if (SHOULD_LOG_DEBUG()) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end_time - start_time);
        OStringStream oss;
        oss << "Query tree optimization time cost: " << duration << std::endl;
        oss << "Query tree after optimization:\n";
        if (optimized_root) {
            optimized_root->PrintTree(oss);
        } else {
            oss << "Empty query tree!\n";
        }
        LOG_DEBUG(std::move(oss).str());
    }
    return optimized_root;
}

std::unique_ptr<QueryNode> MultiQueryNode::GetNewOptimizedQueryTree() {
    for (auto &child : children_) {
        switch (child->GetType()) {
            case QueryNodeType::TERM: {
                // no need to optimize
                break;
            }
            case QueryNodeType::PHRASE: {
                break;
            }
            case QueryNodeType::AND_NOT: {
                String error_message = "GetNewOptimizedQueryTree: Unexpected case! AndNotQueryNode should not exist in parser output";
                UnrecoverableError(error_message);
                break;
            }
            case QueryNodeType::NOT:
            case QueryNodeType::AND:
            case QueryNodeType::OR: {
                // recursively optimize
                child = static_cast<MultiQueryNode *>(child.get())->GetNewOptimizedQueryTree();
                break;
            }
            default: {
                String error_message = "GetNewOptimizedQueryTree: Unexpected case!";
                UnrecoverableError(error_message);
                break;
            }
        }
    }
    // now children are all optimized
    return InnerGetNewOptimizedQueryTree();
}

// 1. deal with "not": "not" must finally combine with parent "and" and turn into "and_not"
// properties of "not":
// 1.1. parent of "not" cannot be "not"
// 1.2. if parent is "and", then "not" should be combined with "and" and turn into "and_not"
// 1.3. if parent is "or", then the children of parent "or" must all be "not", and the "or" should be turned into "not (child and ...)"
// example:
// valid query: "A and (not B)" => "A and_not B"
// valid query: "A and ((not B) and (not C))" => "A and_not (B, C)"
// valid query: "A and ((not B) or (not C))" => "A and_not (B and C)"
// invalid query: "A or (not B)" : can't generate "and_not"
// invalid query: "A and ((not B) or C)" : subexpression "(not B) or C" is invalid
// here it is equivalent to "(A and_not B) or (A and C)", but it is more simple to disallow this case

std::unique_ptr<QueryNode> NotQueryNode::InnerGetNewOptimizedQueryTree() {
    if (children_.empty()) {
        String error_message = "Invalid query statement: NotQueryNode should have at least 1 children";
        UnrecoverableError(error_message);
    }
    auto new_not_node = std::make_unique<NotQueryNode>(); // new node, weight is reset to 1.0
    auto &new_not_list = new_not_node->children_;
    for (auto &child : children_) {
        switch (child->GetType()) {
            case QueryNodeType::NOT: {
                Status status = Status::SyntaxError("Invalid query statement: NotQueryNode should not have not child");
                RecoverableError(status);
                break;
            }
            case QueryNodeType::TERM:
            case QueryNodeType::PHRASE:
            case QueryNodeType::AND:
            case QueryNodeType::AND_NOT: {
                new_not_list.emplace_back(std::move(child));
                break;
            }
            case QueryNodeType::OR: {
                auto &or_node = static_cast<OrQueryNode &>(*child);
                for (auto &or_child : or_node.children_) {
                    new_not_list.emplace_back(std::move(or_child));
                }
                break;
            }
            default: {
                String error_message = "OptimizeInPlaceInner: Unexpected case!";
                UnrecoverableError(error_message);
                break;
            }
        }
    }
    return new_not_node;
}

// 2. deal with "and":
// rule for "and" (in execute order)
// 2.1. for all children,
//      if a child is "and" (it is already optimized, can only contain term or "or"), then flatten it into "and list"
//      if a child is term or "or", move it to "and list"
//      if a child is "not", move it to "not list", deal with it later
//      if a child is "and_not", move first child to "and list" (flatten it if it is an "and"), other children to "not list"
// 2.2. build result:
//      all cases:  "and list" | "not list"
//                       Y     |      Y       => build "and_not"
//                       Y     |      N       => build "and"
//                       N     |      Y       => build "not" of (child or ...)

std::unique_ptr<QueryNode> AndQueryNode::InnerGetNewOptimizedQueryTree() {
    if (children_.size() < 2) {
        String error_message = "Invalid query statement: AndQueryNode should have at least 2 children";
        UnrecoverableError(error_message);
    }
    std::vector<std::unique_ptr<QueryNode>> and_list;
    std::vector<std::unique_ptr<QueryNode>> not_list;
    // 2.1.
    for (auto &child : children_) {
        switch (child->GetType()) {
            case QueryNodeType::AND: {
                auto &and_node = static_cast<AndQueryNode &>(*child);
                and_list.insert(and_list.end(),
                                std::make_move_iterator(and_node.children_.begin()),
                                std::make_move_iterator(and_node.children_.end()));
                break;
            }
            case QueryNodeType::TERM:
            case QueryNodeType::PHRASE:
            case QueryNodeType::OR: {
                and_list.emplace_back(std::move(child));
                break;
            }
            case QueryNodeType::NOT: {
                auto &not_node = static_cast<NotQueryNode &>(*child);
                not_list.insert(not_list.end(),
                                std::make_move_iterator(not_node.children_.begin()),
                                std::make_move_iterator(not_node.children_.end()));
                break;
            }
            case QueryNodeType::AND_NOT: {
                auto &and_not_node = static_cast<AndNotQueryNode &>(*child);
                if (auto &first_child = and_not_node.children_.front(); first_child->GetType() == QueryNodeType::AND) {
                    auto &and_node = static_cast<AndQueryNode &>(*first_child);
                    and_list.insert(and_list.end(),
                                    std::make_move_iterator(and_node.children_.begin()),
                                    std::make_move_iterator(and_node.children_.end()));
                } else {
                    and_list.emplace_back(std::move(first_child));
                }
                not_list.insert(not_list.end(),
                                std::make_move_iterator(std::next(and_not_node.children_.begin())),
                                std::make_move_iterator(and_not_node.children_.end()));
                break;
            }
            default: {
                String error_message = "OptimizeInPlaceInner: Unexpected case!";
                UnrecoverableError(error_message);
                break;
            }
        }
    }
    // 2.2.
    if (and_list.empty()) {
        // at least 2 children
        auto not_node = std::make_unique<NotQueryNode>(); // new node, weight is reset to 1.0
        not_node->children_ = std::move(not_list);
        return not_node;
    } else if (not_list.empty()) {
        // at least 2 children
        auto and_node = std::make_unique<AndQueryNode>(); // new node, weight is reset to 1.0
        and_node->children_ = std::move(and_list);
        return and_node;
    } else {
        auto and_not_node = std::make_unique<AndNotQueryNode>(); // new node, weight is reset to 1.0
        // build first child from and_list
        if (and_list.size() == 1) {
            and_not_node->children_.emplace_back(std::move(and_list.front()));
        } else {
            auto and_node = std::make_unique<AndQueryNode>(); // new node, weight is reset to 1.0
            and_node->children_ = std::move(and_list);
            and_not_node->children_.emplace_back(std::move(and_node));
        }
        // move not_list to and_not_node
        and_not_node->children_.insert(and_not_node->children_.end(),
                                       std::make_move_iterator(not_list.begin()),
                                       std::make_move_iterator(not_list.end()));
        return and_not_node;
    }
}

// 3. deal with "or":
// rule for "or" (in execute order)
// 3.1. for all children,
//      if a child is "or" (it is already optimized, can only contain term, "and" or "and_not"), flatten it into "or list"
//      if a child is term, "and" or "and_not", move it to "or list"
//      if a child is "not", move it to "not list", deal with it later
// 3.2. build result:
//      all cases:  "or list" | "not list"
//                       Y    |      Y       => invalid query
//                       Y    |      N       => build "or"
//                       N    |      Y       => build "not" of (child and ...), here child can be term, "and", "and_not" or "or", need optimization

std::unique_ptr<QueryNode> OrQueryNode::InnerGetNewOptimizedQueryTree() {
    if (children_.size() < 2) {
        String error_message = "OptimizeInPlaceInner: Unexpected case! AndNotQueryNode should not exist in parser output";
        UnrecoverableError(error_message);
    }
    std::vector<std::unique_ptr<QueryNode>> or_list;
    std::vector<std::unique_ptr<QueryNode>> not_list;
    // 3.1.
    for (auto &child : children_) {
        switch (child->GetType()) {
            case QueryNodeType::OR: {
                auto &or_node = static_cast<OrQueryNode &>(*child);
                or_list.insert(or_list.end(), std::make_move_iterator(or_node.children_.begin()), std::make_move_iterator(or_node.children_.end()));
                break;
            }
            case QueryNodeType::TERM:
            case QueryNodeType::PHRASE:
            case QueryNodeType::AND:
            case QueryNodeType::AND_NOT: {
                or_list.emplace_back(std::move(child));
                break;
            }
            case QueryNodeType::NOT: {
                not_list.emplace_back(std::move(child));
                break;
            }
            default: {
                String error_message = "OptimizeInPlaceInner: Unexpected case!";
                UnrecoverableError(error_message);
                break;
            }
        }
    }
    // 3.2.
    if (or_list.empty()) {
        // at least 2 children
        // build "not" of (child and ...), here child can be term, "and", "and_not" or "or" (reconstructed from "not" children), need optimization
        // children of optimized "not" (form an implicit "or" node) can only be term, "and" or "and_not"
        auto not_node = std::make_unique<NotQueryNode>(); // new node, weight is reset to 1.0
        auto and_node = std::make_unique<AndQueryNode>(); // new node, weight is reset to 1.0
        for (auto &not_child : not_list) {
            auto &not_child_node = static_cast<NotQueryNode &>(*not_child);
            if (not_child_node.children_.size() == 1) {
                // here child can be term, "and" or "and_not"
                and_node->children_.emplace_back(std::move(not_child_node.children_.front()));
            } else {
                // rebuild "or" from "not" children list
                // the new "or" node is optimized
                auto or_node = std::make_unique<OrQueryNode>(); // new node, weight is reset to 1.0
                or_node->children_ = std::move(not_child_node.children_);
                and_node->children_.emplace_back(std::move(or_node));
            }
        }
        // reuse the "and" rule to optimize
        // optimized_node possible type: "and", "and_not"
        auto optimized_node = and_node->InnerGetNewOptimizedQueryTree();
        not_node->children_.emplace_back(std::move(optimized_node));
        return not_node;
    } else if (not_list.empty()) {
        // merge duplicated TermQueryNode children
        std::vector<std::unique_ptr<QueryNode>> or_list_tmp;
        for (auto &child1 : or_list) {
            if (child1->GetType() != QueryNodeType::TERM) {
                or_list_tmp.emplace_back(std::move(child1));
                continue;
            }
            TermQueryNode *term_query_node1 = static_cast<TermQueryNode *>(child1.get());
            bool duplicated = false;
            for (auto &child2 : or_list_tmp) {
                if (child2->GetType() == QueryNodeType::TERM) {
                    TermQueryNode *term_query_node2 = static_cast<TermQueryNode *>(child2.get());
                    if (term_query_node1->term_ == term_query_node2->term_ && term_query_node1->column_ == term_query_node2->column_ &&
                        term_query_node1->position_ == term_query_node2->position_) {
                        term_query_node2->weight_ += term_query_node1->weight_;
                        duplicated = true;
                        break;
                    }
                }
            }
            if (!duplicated) {
                or_list_tmp.emplace_back(std::move(child1));
            }
        }
        or_list = std::move(or_list_tmp);

        if (or_list.size() == 1) {
            return std::move(or_list[0]);
        }
        // at least 2 children
        auto or_node = std::make_unique<OrQueryNode>(); // new node, weight is reset to 1.0
        or_node->children_ = std::move(or_list);
        return or_node;
    } else {
        Status status = Status::SyntaxError("Invalid query statement: OrQueryNode should not have both not child and non-not child");
        RecoverableError(status);
        return nullptr;
    }
}

// 4. deal with "and_not":
// "and_not" does not exist in parser output, it is generated during optimization

std::unique_ptr<QueryNode> AndNotQueryNode::InnerGetNewOptimizedQueryTree() {
    String error_message = "OptimizeInPlaceInner: Unexpected case! AndNotQueryNode should not exist in parser output";
    UnrecoverableError(error_message);
    return nullptr;
}

// create search iterator
std::unique_ptr<DocIterator> TermQueryNode::CreateSearch(const TableEntry *table_entry,
                                                         const IndexReader &index_reader,
                                                         EarlyTermAlgo /*early_term_algo*/,
                                                         u32 /*minimum_should_match*/) const {
    ColumnID column_id = table_entry->GetColumnIdByName(column_);
    ColumnIndexReader *column_index_reader = index_reader.GetColumnIndexReader(column_id);
    if (!column_index_reader) {
        RecoverableError(Status::SyntaxError(fmt::format(R"(Invalid query statement: Column "{}" has no fulltext index)", column_)));
        return nullptr;
    }

    bool fetch_position = false;
    auto option_flag = column_index_reader->GetOptionFlag();
    if (option_flag & OptionFlag::of_position_list) {
        fetch_position = true;
    }
    auto posting_iterator = column_index_reader->Lookup(term_, fetch_position);
    if (!posting_iterator) {
        return nullptr;
    }
    auto search = MakeUnique<TermDocIterator>(std::move(posting_iterator), column_id, GetWeight());
    auto column_length_reader = MakeUnique<FullTextColumnLengthReader>(column_index_reader);
    search->InitBM25Info(std::move(column_length_reader));
    search->term_ptr_ = &term_;
    search->column_name_ptr_ = &column_;
    return search;
}

std::unique_ptr<DocIterator> PhraseQueryNode::CreateSearch(const TableEntry *table_entry,
                                                           const IndexReader &index_reader,
                                                           EarlyTermAlgo /*early_term_algo*/,
                                                           u32 /*minimum_should_match*/) const {
    ColumnID column_id = table_entry->GetColumnIdByName(column_);
    ColumnIndexReader *column_index_reader = index_reader.GetColumnIndexReader(column_id);
    if (!column_index_reader) {
        RecoverableError(Status::SyntaxError(fmt::format(R"(Invalid query statement: Column "{}" has no fulltext index)", column_)));
        return nullptr;
    }
    bool fetch_position = false;
    auto option_flag = column_index_reader->GetOptionFlag();
    if (option_flag & OptionFlag::of_position_list) {
        fetch_position = true;
    }
    Vector<std::unique_ptr<PostingIterator>> posting_iterators;
    for (auto &term : terms_) {
        auto posting_iterator = column_index_reader->Lookup(term, fetch_position);
        if (nullptr == posting_iterator) {
            return nullptr;
        }
        posting_iterators.emplace_back(std::move(posting_iterator));
    }
    auto search = MakeUnique<PhraseDocIterator>(std::move(posting_iterators), GetWeight(), slop_);
    auto column_length_reader = MakeUnique<FullTextColumnLengthReader>(column_index_reader);
    search->InitBM25Info(std::move(column_length_reader));
    search->terms_ptr_ = &terms_;
    search->column_name_ptr_ = &column_;
    return search;
}

std::unique_ptr<DocIterator> AndQueryNode::CreateSearch(const TableEntry *table_entry,
                                                        const IndexReader &index_reader,
                                                        const EarlyTermAlgo early_term_algo,
                                                        const u32 minimum_should_match) const {
    Vector<std::unique_ptr<DocIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    for (auto &child : children_) {
        auto iter = child->CreateSearch(table_entry, index_reader, early_term_algo, 0);
        if (!iter) {
            // no need to continue if any child is invalid
            return nullptr;
        }
        sub_doc_iters.emplace_back(std::move(iter));
    }
    if (sub_doc_iters.empty()) {
        return nullptr;
    } else if (sub_doc_iters.size() == 1) {
        return std::move(sub_doc_iters[0]);
    } else if (minimum_should_match <= sub_doc_iters.size()) {
        return MakeUnique<AndIterator>(std::move(sub_doc_iters));
    } else {
        assert(minimum_should_match > 2u);
        return MakeUnique<MinimumShouldMatchWrapper<AndIterator>>(std::move(sub_doc_iters), minimum_should_match);
    }
}

std::unique_ptr<DocIterator> AndNotQueryNode::CreateSearch(const TableEntry *table_entry,
                                                           const IndexReader &index_reader,
                                                           const EarlyTermAlgo early_term_algo,
                                                           const u32 minimum_should_match) const {
    Vector<std::unique_ptr<DocIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    // check if the first child is a valid query
    auto first_iter = children_.front()->CreateSearch(table_entry, index_reader, early_term_algo, minimum_should_match);
    if (!first_iter) {
        // no need to continue if the first child is invalid
        return nullptr;
    }
    sub_doc_iters.emplace_back(std::move(first_iter));
    for (u32 i = 1; i < children_.size(); ++i) {
        auto iter = children_[i]->CreateSearch(table_entry, index_reader, early_term_algo, 0);
        if (iter) {
            sub_doc_iters.emplace_back(std::move(iter));
        }
    }
    if (sub_doc_iters.size() == 1) {
        return std::move(sub_doc_iters[0]);
    } else {
        return MakeUnique<AndNotIterator>(std::move(sub_doc_iters));
    }
}

std::unique_ptr<DocIterator> OrQueryNode::CreateSearch(const TableEntry *table_entry,
                                                       const IndexReader &index_reader,
                                                       const EarlyTermAlgo early_term_algo,
                                                       const u32 minimum_should_match) const {
    Vector<std::unique_ptr<DocIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    bool all_are_term = true;
    bool all_are_term_or_phrase = true;
    const QueryNode *only_child_ = nullptr;
    for (auto &child : children_) {
        if (auto iter = child->CreateSearch(table_entry, index_reader, early_term_algo, 0); iter) {
            only_child_ = child.get();
            sub_doc_iters.emplace_back(std::move(iter));
            if (const auto child_type = child->GetType(); child_type != QueryNodeType::TERM) {
                all_are_term = false;
                if (child_type != QueryNodeType::PHRASE) {
                    all_are_term_or_phrase = false;
                }
            }
        }
    }
    if (sub_doc_iters.empty()) {
        return nullptr;
    } else if (sub_doc_iters.size() == 1) {
        return only_child_->CreateSearch(table_entry, index_reader, early_term_algo, minimum_should_match);
    } else if (all_are_term && early_term_algo == EarlyTermAlgo::kBMW) {
        if (minimum_should_match <= 1u) {
            return MakeUnique<BlockMaxWandIterator>(std::move(sub_doc_iters));
        } else if (minimum_should_match < sub_doc_iters.size()) {
            return MakeUnique<MinimumShouldMatchWrapper<BlockMaxWandIterator>>(std::move(sub_doc_iters), minimum_should_match);
        } else if (minimum_should_match == sub_doc_iters.size()) {
            return MakeUnique<AndIterator>(std::move(sub_doc_iters));
        } else {
            return nullptr;
        }
    } else if (all_are_term_or_phrase) {
        if (minimum_should_match <= 1u) {
            return MakeUnique<OrIterator>(std::move(sub_doc_iters));
        } else if (minimum_should_match < sub_doc_iters.size()) {
            return MakeUnique<MinimumShouldMatchIterator>(std::move(sub_doc_iters), minimum_should_match);
        } else if (minimum_should_match == sub_doc_iters.size()) {
            return MakeUnique<AndIterator>(std::move(sub_doc_iters));
        } else {
            return nullptr;
        }
    } else {
        if (minimum_should_match <= 1u) {
            return MakeUnique<OrIterator>(std::move(sub_doc_iters));
        } else {
            return MakeUnique<MinimumShouldMatchWrapper<OrIterator>>(std::move(sub_doc_iters));
        }
    }
}

std::unique_ptr<DocIterator> NotQueryNode::CreateSearch(const TableEntry *table_entry,
                                                        const IndexReader &index_reader,
                                                        EarlyTermAlgo early_term_algo,
                                                        u32 minimum_should_match) const {
    UnrecoverableError("NOT query node should be optimized into AND_NOT query node");
    return nullptr;
}

// print tree

std::string QueryNodeTypeToString(QueryNodeType type) {
    switch (type) {
        case QueryNodeType::INVALID:
            return "INVALID";
        case QueryNodeType::FILTER:
            return "FILTER";
        case QueryNodeType::TERM:
            return "TERM";
        case QueryNodeType::AND:
            return "AND";
        case QueryNodeType::AND_NOT:
            return "AND_NOT";
        case QueryNodeType::OR:
            return "OR";
        case QueryNodeType::NOT:
            return "NOT";
        case QueryNodeType::PHRASE:
            return "PHRASE";
        case QueryNodeType::PREFIX_TERM:
            return "PREFIX_TERM";
        case QueryNodeType::SUFFIX_TERM:
            return "SUFFIX_TERM";
        case QueryNodeType::SUBSTRING_TERM:
            return "SUBSTRING_TERM";
    }
}

void TermQueryNode::PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << QueryNodeTypeToString(type_);
    os << " (weight: " << weight_ << ")";
    os << " (column: " << column_ << ")";
    os << " (term: " << term_ << ")";
    os << '\n';
}

void TermQueryNode::GetQueryColumnsTerms(std::vector<std::string> &columns, std::vector<std::string> &terms) const {
    columns.push_back(column_);
    terms.push_back(term_);
}

void PhraseQueryNode::PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << QueryNodeTypeToString(type_);
    os << " (weight: " << weight_ << ")";
    os << " (column: " << column_ << ")";
    os << " (phrase:";
    for (auto term : terms_) {
        os << " " << term;
    }
    os << ")";
    os << " (slop: " << slop_ << ")";
    os << '\n';
}

void PhraseQueryNode::GetQueryColumnsTerms(std::vector<std::string> &columns, std::vector<std::string> &terms) const {
    columns.push_back(column_);
    for (auto term : terms_) {
        terms.push_back(term);
    }
}

void MultiQueryNode::PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << QueryNodeTypeToString(type_);
    os << " (weight: " << weight_ << ")";
    os << " (children count: " << children_.size() << ")";
    os << '\n';
    std::string next_prefix = prefix + (is_final ? "    " : "│   ");
    for (u32 i = 0; i + 1 < children_.size(); ++i) {
        children_[i]->PrintTree(os, next_prefix, false);
    }
    children_.back()->PrintTree(os, next_prefix, true);
}

void MultiQueryNode::GetQueryColumnsTerms(std::vector<std::string> &columns, std::vector<std::string> &terms) const {
    for (u32 i = 0; i < children_.size(); ++i) {
        children_[i]->GetQueryColumnsTerms(columns, terms);
    }
}

uint32_t MultiQueryNode::LeafCount() const {
    if (GetType() != QueryNodeType::OR && GetType() != QueryNodeType::AND) {
        UnrecoverableError("LeafCount: Unexpected case!");
    }
    return std::accumulate(children_.begin(), children_.end(), static_cast<u32>(0), [](const u32 cnt, const auto &it) {
        return cnt + it->LeafCount();
    });
}


} // namespace infinity
