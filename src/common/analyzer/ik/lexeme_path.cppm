module;

#include <sstream>
#include <stdexcept>

export module lexeme_path;

import stl;
import quick_sort_set;
import lexeme;

namespace infinity {

export class LexemePath : public QuickSortSet {
public:
    int path_begin_;

    int path_end_;

    int payload_length_;

    LexemePath();

    bool AddCrossLexeme(Lexeme *lexeme);

    bool AddNotCrossLexeme(Lexeme *lexeme);

    Lexeme *RemoveTail();

    bool CheckCross(Lexeme *lexeme);

    int GetPathBegin() const { return path_begin_; }

    int GetPathEnd() const { return path_end_; }

    int GetPayloadLength() const { return this->payload_length_; }

    int GetPathLength() const { return this->path_end_ - this->path_begin_; }

    int GetXWeight() const;

    int GetPWeight() const;

    LexemePath *Copy() const;

    int CompareTo(const LexemePath &o) const;

    std::string ToString() const;
};
} // namespace infinity