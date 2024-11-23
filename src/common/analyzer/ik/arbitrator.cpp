module;

#include <stack>

import stl;
import analyze_context;
import lexeme;
import lexeme_path;
import quick_sort_set;

module arbitrator;

namespace infinity {

void IKArbitrator::Process(AnalyzeContext *context, bool use_smart) {
    QuickSortSet *org_lexemes = context->GetOrgLexemes();
    Lexeme *org_lexeme = org_lexemes->PollFirst();

    LexemePath *cross_path = new LexemePath();
    while (org_lexeme != nullptr) {
        if (!cross_path->AddCrossLexeme(org_lexeme)) {
            if (cross_path->Size() == 1 || !use_smart) {
                context->AddLexemePath(cross_path);
            } else {
                QuickSortSet::Cell *headCell = cross_path->GetHead();
                LexemePath *judge_result = Judge(headCell, cross_path->GetPathLength());
                context->AddLexemePath(judge_result);
            }

            cross_path = new LexemePath();
            cross_path->AddCrossLexeme(org_lexeme);
        }
        org_lexeme = org_lexemes->PollFirst();
    }

    if (cross_path->Size() == 1 || !use_smart) {
        context->AddLexemePath(cross_path);
    } else {
        QuickSortSet::Cell *headCell = cross_path->GetHead();
        LexemePath *judge_result = Judge(headCell, cross_path->GetPathLength());
        context->AddLexemePath(judge_result);
    }
}

LexemePath *IKArbitrator::Judge(QuickSortSet::Cell *lexeme_cell, int fullTextLength) {
    std::set<LexemePath *> path_options;
    LexemePath *option = new LexemePath();

    std::stack<QuickSortSet::Cell *> lexeme_stack = ForwardPath(lexeme_cell, option);

    path_options.insert(option->Copy());

    QuickSortSet::Cell *c = nullptr;
    while (!lexeme_stack.empty()) {
        c = lexeme_stack.top();
        lexeme_stack.pop();
        BackPath(c->GetLexeme(), option);
        ForwardPath(c, option);
        path_options.insert(option->Copy());
    }

    return *path_options.begin();
}

std::stack<QuickSortSet::Cell *> IKArbitrator::ForwardPath(QuickSortSet::Cell *lexeme_cell, LexemePath *option) {
    std::stack<QuickSortSet::Cell *> conflict_stack;
    QuickSortSet::Cell *c = lexeme_cell;
    while (c != nullptr && c->GetLexeme() != nullptr) {
        if (!option->AddNotCrossLexeme(c->GetLexeme())) {
            conflict_stack.push(c);
        }
        c = c->GetNext();
    }
    return conflict_stack;
}

void IKArbitrator::BackPath(Lexeme *l, LexemePath *option) {
    while (option->CheckCross(l)) {
        option->RemoveTail();
    }
}

} // namespace infinity