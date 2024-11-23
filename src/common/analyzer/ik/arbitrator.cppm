module;

#include <stack>

export module arbitrator;

import stl;
import analyze_context;
import lexeme;
import lexeme_path;
import quick_sort_set;

namespace infinity {

export class IKArbitrator {
public:
    IKArbitrator() {}

    void Process(AnalyzeContext *context, bool use_smart);

private:
    LexemePath *Judge(QuickSortSet::Cell *lexeme_cell, int fullTextLength);

    std::stack<QuickSortSet::Cell *> ForwardPath(QuickSortSet::Cell *lexeme_cell, LexemePath *option);

    void BackPath(Lexeme *l, LexemePath *option);
};
} // namespace infinity