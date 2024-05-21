#include "query_node.h"
#include <chrono>

import stl;
import status;
import infinity_exception;
import logger;
import doc_iterator;
import term_doc_iterator;
import and_iterator;
import and_not_iterator;
import or_iterator;
import table_entry;
import column_index_reader;
import match_data;
import posting_iterator;
import early_terminate_iterator;
import blockmax_term_doc_iterator;
import blockmax_and_iterator;
import blockmax_and_not_iterator;
import blockmax_wand_iterator;
import blockmax_maxscore_iterator;
import index_defines;
import third_party;
import phrase_doc_iterator;
import blockmax_phrase_doc_iterator;

namespace infinity {

// optimize: from leaf to root, replace tree node in place

// expected property of optimized node:
// 1. children of "not" can only be term, "and" or "and_not", because "not" is not allowed, and "or" will be flattened to not list
// 2. children of "and" can only be term or "or", because "and", "not", "and_not" will be optimized
// 3. children of "or" can only be term, "and" or "and_not", because "or" will be optimized, and "not" is either optimized or not allowed
// 4. "and_not" does not exist in parser output, it is generated during optimization
//    "and_not": first child can be term, "and", "or", other children form a list of "not"
void QueryNode::FilterOptimizeQueryTree() { UnrecoverableError("Should not reach here!"); }
std::unique_ptr<QueryNode> QueryNode::GetOptimizedQueryTree(std::unique_ptr<QueryNode> root) {
    if (root->GetType() == QueryNodeType::FILTER) {
        root->FilterOptimizeQueryTree();
        return root;
    }
#ifdef INFINITY_DEBUG
    auto start_time = std::chrono::high_resolution_clock::now();
#endif
    std::unique_ptr<QueryNode> optimized_root;
    if (!root) {
        Status status = Status::SyntaxError("Invalid query statement: Empty query tree");
        LOG_ERROR(status.message());
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
            LOG_ERROR(status.message());
            RecoverableError(status);
            break;
        }
        case QueryNodeType::AND:
        case QueryNodeType::OR: {
            optimized_root = static_cast<MultiQueryNode *>(root.get())->GetNewOptimizedQueryTree();
            if (optimized_root->GetType() == QueryNodeType::NOT) {
                Status status = Status::SyntaxError("Invalid query statement: NotQueryNode should not be on the top level");
                LOG_ERROR(status.message());
                RecoverableError(status);
            }
            break;
        }
        case QueryNodeType::AND_NOT: {
            UnrecoverableError("Unexpected AndNotQueryNode from parser output");
            break;
        }
        default: {
            UnrecoverableError("GetOptimizedQueryTree: Unexpected case!");
            break;
        }
    }
#ifdef INFINITY_DEBUG
    {
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
#endif
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
                UnrecoverableError("GetNewOptimizedQueryTree: Unexpected case! AndNotQueryNode should not exist in parser output");
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
                UnrecoverableError("GetNewOptimizedQueryTree: Unexpected case!");
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
        UnrecoverableError("Invalid query statement: NotQueryNode should have at least 1 child");
    }
    auto new_not_node = std::make_unique<NotQueryNode>(); // new node, weight is reset to 1.0
    auto &new_not_list = new_not_node->children_;
    for (auto &child : children_) {
        switch (child->GetType()) {
            case QueryNodeType::NOT: {
                Status status = Status::SyntaxError("Invalid query statement: NotQueryNode should not have not child");
                LOG_ERROR(status.message());
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
                UnrecoverableError("OptimizeInPlaceInner: Unexpected case!");
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
        UnrecoverableError("Invalid query statement: AndQueryNode should have at least 2 children");
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
                UnrecoverableError("OptimizeInPlaceInner: Unexpected case!");
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
        UnrecoverableError("Invalid query statement: OrQueryNode should have at least 2 children");
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
                UnrecoverableError("OptimizeInPlaceInner: Unexpected case!");
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
        // at least 2 children
        auto or_node = std::make_unique<OrQueryNode>(); // new node, weight is reset to 1.0
        or_node->children_ = std::move(or_list);
        return or_node;
    } else {
        Status status = Status::SyntaxError("Invalid query statement: OrQueryNode should not have both not child and non-not child");
        LOG_ERROR(status.message());
        RecoverableError(status);
        return nullptr;
    }
}

// 4. deal with "and_not":
// "and_not" does not exist in parser output, it is generated during optimization

std::unique_ptr<QueryNode> AndNotQueryNode::InnerGetNewOptimizedQueryTree() {
    UnrecoverableError("OptimizeInPlaceInner: Unexpected case! AndNotQueryNode should not exist in parser output");
    return nullptr;
}

// create search iterator
std::unique_ptr<DocIterator> TermQueryNode::CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const {
    ColumnID column_id = table_entry->GetColumnIdByName(column_);
    ColumnIndexReader *column_index_reader = index_reader.GetColumnIndexReader(column_id);
    if (!column_index_reader) {
        return nullptr;
    }

    bool fetch_position = false;
    auto option_flag = column_index_reader->GetOptionFlag();
    if (option_flag & OptionFlag::of_position_list) {
        fetch_position = true;
    }
    auto posting_iterator = column_index_reader->Lookup(term_, index_reader.session_pool_.get(), fetch_position);
    if (!posting_iterator) {
        return nullptr;
    }
    auto search = MakeUnique<TermDocIterator>(std::move(posting_iterator), column_id, GetWeight());
    search->term_ptr_ = &term_;
    search->column_name_ptr_ = &column_;
    if (scorer) {
        // nodes under "not" will not be added to scorer
        scorer->AddDocIterator(search.get(), column_id);
    }
    return search;
}

std::unique_ptr<EarlyTerminateIterator> TermQueryNode::CreateEarlyTerminateSearch(const TableEntry *table_entry,
                                                                                  IndexReader &index_reader,
                                                                                  Scorer *scorer,
                                                                                  EarlyTermAlg /*early_term_alg*/) const {
    ColumnID column_id = table_entry->GetColumnIdByName(column_);
    ColumnIndexReader *column_index_reader = index_reader.GetColumnIndexReader(column_id);
    if (!column_index_reader) {
        return nullptr;
    }
    bool fetch_position = false;
    auto option_flag = column_index_reader->GetOptionFlag();
    if (option_flag & OptionFlag::of_position_list) {
        fetch_position = true;
    }
    auto search = column_index_reader->LookupBlockMax(term_, index_reader.session_pool_.get(), GetWeight(), fetch_position);
    if (!search) {
        return nullptr;
    }
    search->term_ptr_ = &term_;
    search->column_name_ptr_ = &column_;
    if (scorer) {
        // nodes under "not" will not be added to scorer
        scorer->AddBlockMaxDocIterator(search.get(), column_id);
    }
    return search;
}

std::unique_ptr<DocIterator> PhraseQueryNode::CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const {
    ColumnID column_id = table_entry->GetColumnIdByName(column_);
    ColumnIndexReader *column_index_reader = index_reader.GetColumnIndexReader(column_id);
    if (!column_index_reader) {
        return nullptr;
    }
    bool fetch_position = false;
    auto option_flag = column_index_reader->GetOptionFlag();
    if (option_flag & OptionFlag::of_position_list) {
        fetch_position = true;
    }
    Vector<std::unique_ptr<PostingIterator>> posting_iterators;
    for (auto &term : terms_) {
        auto posting_iterator = column_index_reader->Lookup(term, index_reader.session_pool_.get(), fetch_position);
        if (nullptr == posting_iterator) {
            return nullptr;
        }
        posting_iterators.emplace_back(std::move(posting_iterator));
    }
    auto search = MakeUnique<PhraseDocIterator>(std::move(posting_iterators), GetWeight());

    search->terms_ptr_ = &terms_;
    search->column_name_ptr_ = &column_;

    if (scorer) {
        scorer->AddDocIterator(search.get(), column_id);
    }
    return search;
}

std::unique_ptr<EarlyTerminateIterator> PhraseQueryNode::CreateEarlyTerminateSearch(const TableEntry *table_entry,
                                                                                    IndexReader &index_reader,
                                                                                    Scorer *scorer,
                                                                                    EarlyTermAlg /*early_term_alg*/) const {
    ColumnID column_id = table_entry->GetColumnIdByName(column_);
    ColumnIndexReader *column_index_reader = index_reader.GetColumnIndexReader(column_id);
    if (!column_index_reader) {
        return nullptr;
    }
    bool fetch_position = false;
    auto option_flag = column_index_reader->GetOptionFlag();
    if (option_flag & OptionFlag::of_position_list) {
        fetch_position = true;
    }

    Vector<std::unique_ptr<PostingIterator>> posting_iterators;
    for (auto& term : terms_) {
        auto posting_iterator = column_index_reader->Lookup(term, index_reader.session_pool_.get(), fetch_position);
        if (nullptr == posting_iterator) {
            fmt::print("not found term = {}\n", term);
            return nullptr;
        }
        posting_iterators.emplace_back(std::move(posting_iterator));
    }
    auto search = MakeUnique<BlockMaxPhraseDocIterator>(std::move(posting_iterators), GetWeight());
    if (!search) {
        return nullptr;
    }
    search->terms_ptr_ = &terms_;
    search->column_name_ptr_ = &column_;
    if (scorer) {
        // nodes under "not" will not be added to scorer
        scorer->AddBlockMaxPhraseDocIterator(search.get(), column_id);
    }
    return search;
}

std::unique_ptr<DocIterator> AndQueryNode::CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const {
    Vector<std::unique_ptr<DocIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    for (auto &child : children_) {
        auto iter = child->CreateSearch(table_entry, index_reader, scorer);
        if (iter) {
            sub_doc_iters.emplace_back(std::move(iter));
        }
    }
    if (sub_doc_iters.empty()) {
        return nullptr;
    } else if (sub_doc_iters.size() == 1) {
        return std::move(sub_doc_iters[0]);
    } else {
        return MakeUnique<AndIterator>(std::move(sub_doc_iters));
    }
}

std::unique_ptr<EarlyTerminateIterator> AndQueryNode::CreateEarlyTerminateSearch(const TableEntry *table_entry,
                                                                                 IndexReader &index_reader,
                                                                                 Scorer *scorer,
                                                                                 EarlyTermAlg early_term_alg) const {
    Vector<std::unique_ptr<EarlyTerminateIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    for (auto &child : children_) {
        auto iter = child->CreateEarlyTerminateSearch(table_entry, index_reader, scorer, early_term_alg);
        if (iter) {
            sub_doc_iters.emplace_back(std::move(iter));
        }
    }
    if (sub_doc_iters.empty()) {
        return nullptr;
    } else if (sub_doc_iters.size() == 1) {
        return std::move(sub_doc_iters[0]);
    } else {
        return MakeUnique<BlockMaxAndIterator>(std::move(sub_doc_iters));
    }
}

std::unique_ptr<DocIterator> AndNotQueryNode::CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const {
    Vector<std::unique_ptr<DocIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    // check if the first child is a valid query
    auto first_iter = children_.front()->CreateSearch(table_entry, index_reader, scorer);
    if (!first_iter) {
        // no need to continue if the first child is invalid
        return nullptr;
    }
    sub_doc_iters.emplace_back(std::move(first_iter));
    for (u32 i = 1; i < children_.size(); ++i) {
        // set scorer to nullptr, because nodes under "not" should not be added to scorer
        auto iter = children_[i]->CreateSearch(table_entry, index_reader, nullptr);
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

std::unique_ptr<EarlyTerminateIterator> AndNotQueryNode::CreateEarlyTerminateSearch(const TableEntry *table_entry,
                                                                                    IndexReader &index_reader,
                                                                                    Scorer *scorer,
                                                                                    EarlyTermAlg early_term_alg) const {
    Vector<std::unique_ptr<EarlyTerminateIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    // check if the first child is a valid query
    auto first_iter = children_.front()->CreateEarlyTerminateSearch(table_entry, index_reader, scorer, early_term_alg);
    if (!first_iter) {
        // no need to continue if the first child is invalid
        return nullptr;
    }
    sub_doc_iters.emplace_back(std::move(first_iter));
    for (u32 i = 1; i < children_.size(); ++i) {
        // set scorer to nullptr, because nodes under "not" should not be added to scorer
        auto iter = children_[i]->CreateEarlyTerminateSearch(table_entry, index_reader, nullptr, early_term_alg);
        if (iter) {
            sub_doc_iters.emplace_back(std::move(iter));
        }
    }
    if (sub_doc_iters.size() == 1) {
        return std::move(sub_doc_iters[0]);
    } else {
        return MakeUnique<BlockMaxAndNotIterator>(std::move(sub_doc_iters));
    }
}

std::unique_ptr<DocIterator> OrQueryNode::CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const {
    Vector<std::unique_ptr<DocIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    for (auto &child : children_) {
        auto iter = child->CreateSearch(table_entry, index_reader, scorer);
        if (iter) {
            sub_doc_iters.emplace_back(std::move(iter));
        }
    }
    if (sub_doc_iters.empty()) {
        return nullptr;
    } else if (sub_doc_iters.size() == 1) {
        return std::move(sub_doc_iters[0]);
    } else {
        return MakeUnique<OrIterator>(std::move(sub_doc_iters));
    }
}

std::unique_ptr<EarlyTerminateIterator>
OrQueryNode::CreateEarlyTerminateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer, EarlyTermAlg early_term_alg) const {
    Vector<std::unique_ptr<EarlyTerminateIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    for (auto &child : children_) {
        auto iter = child->CreateEarlyTerminateSearch(table_entry, index_reader, scorer, early_term_alg);
        if (iter) {
            sub_doc_iters.emplace_back(std::move(iter));
        }
    }
    if (sub_doc_iters.empty()) {
        return nullptr;
    } else if (sub_doc_iters.size() == 1) {
        return std::move(sub_doc_iters[0]);
    } else {
        switch (early_term_alg) {
            case EarlyTermAlg::kBMM:
                return MakeUnique<BlockMaxMaxscoreIterator>(std::move(sub_doc_iters));
            case EarlyTermAlg::kBMW:
            default:
                return MakeUnique<BlockMaxWandIterator>(std::move(sub_doc_iters));
        }
    }
}

std::unique_ptr<DocIterator> NotQueryNode::CreateSearch(const TableEntry *table_entry, IndexReader &index_reader, Scorer *scorer) const {
    UnrecoverableError("NOT query node should be optimized into AND_NOT query node");
    return nullptr;
}

std::unique_ptr<EarlyTerminateIterator> NotQueryNode::CreateEarlyTerminateSearch(const TableEntry *table_entry,
                                                                                 IndexReader &index_reader,
                                                                                 Scorer *scorer,
                                                                                 EarlyTermAlg early_term_alg) const {
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
        case QueryNodeType::WAND:
            return "WAND";
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

void PhraseQueryNode::PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << QueryNodeTypeToString(type_);
    os << " (weight: " << weight_ << ")";
    os << " (column: " << column_ << ")";
    os << " (phrase: ";
    for (auto term : terms_) {
        os << term << " ";
    }
    os << ")";
    os << '\n';
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

} // namespace infinity
