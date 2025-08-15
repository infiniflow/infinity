module infinity_core:arbitrator.impl;

import :arbitrator;
import :analyze_context;
import :lexeme;
import :lexeme_path;
import :quick_sort_set;

import std;

namespace infinity {

void IKArbitrator::Process(AnalyzeContext *context, bool use_smart) {
    QuickSortSet *org_lexemes = context->GetOrgLexemes();
    Lexeme *org_lexeme = org_lexemes->PollFirst();

    std::unique_ptr<LexemePath> cross_path = std::make_unique<LexemePath>();
    while (org_lexeme != nullptr) {
        if (!cross_path->AddCrossLexeme(org_lexeme)) {
            if (cross_path->Size() == 1 || !use_smart) {
                context->AddLexemePath(cross_path.release());
            } else {
                QuickSortSet::Cell *head_cell = cross_path->GetHead();
                LexemePath *judge_result = Judge(head_cell, cross_path->GetPathLength());
                context->AddLexemePath(judge_result);
            }
            cross_path = std::make_unique<LexemePath>();
            cross_path->AddCrossLexeme(org_lexeme);
        }
        org_lexeme = org_lexemes->PollFirst();
    }

    if (cross_path->Size() == 1 || !use_smart) {
        context->AddLexemePath(cross_path.release());
    } else {
        QuickSortSet::Cell *head_cell = cross_path->GetHead();
        LexemePath *judge_result = Judge(head_cell, cross_path->GetPathLength());
        context->AddLexemePath(judge_result);
    }
}

struct CompareLexemePath {
    bool operator()(const std::unique_ptr<LexemePath> &lhs, const std::unique_ptr<LexemePath> &rhs) const { return lhs->CompareTo(*rhs); }
};

LexemePath *IKArbitrator::Judge(QuickSortSet::Cell *lexeme_cell, int fulltext_length) {
    std::set<std::unique_ptr<LexemePath>, CompareLexemePath> path_options;
    std::unique_ptr<LexemePath> option = std::make_unique<LexemePath>();

    std::stack<QuickSortSet::Cell *> lexeme_stack = ForwardPath(lexeme_cell, option.get());

    path_options.insert(std::unique_ptr<LexemePath>(option->Copy()));

    QuickSortSet::Cell *c = nullptr;
    while (!lexeme_stack.empty()) {
        c = lexeme_stack.top();
        lexeme_stack.pop();
        BackPath(c->GetLexeme(), option.get());
        ForwardPath(c, option.get());
        path_options.insert(std::unique_ptr<LexemePath>(option->Copy()));
    }
    std::unique_ptr<LexemePath> ret = std::move(path_options.extract(path_options.begin()).value());
    return ret.release();
}

std::stack<QuickSortSet::Cell *> IKArbitrator::ForwardPath(QuickSortSet::Cell *lexeme_cell, LexemePath *option) {
    std::stack<QuickSortSet::Cell *> conflict_stack;
    QuickSortSet::Cell *c = lexeme_cell;
    while (c != nullptr && c->GetLexeme() != nullptr) {
        Lexeme *lexeme = c->GetLexeme()->Copy();
        if (!option->AddNotCrossLexeme(lexeme)) {
            delete lexeme;
            conflict_stack.push(c);
        }
        c = c->GetNext();
    }
    return conflict_stack;
}

void IKArbitrator::BackPath(Lexeme *l, LexemePath *option) {
    while (option->CheckCross(l)) {
        Lexeme *lexeme = option->RemoveTail();
        delete lexeme;
    }
}

} // namespace infinity