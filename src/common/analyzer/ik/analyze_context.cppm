module;

export module infinity_core:analyze_context;

import :stl;
import :quick_sort_set;
import :character_util;
import :lexeme;
import :lexeme_path;
import :ik_dict;

import std;

namespace infinity {

export class AnalyzeContext {
public:
    static constexpr int BUFF_SIZE = 4096;

    static constexpr int BUFF_EXHAUST_CRITICAL = 100;

    Vector<wchar_t> segment_buff_;

    Vector<int> char_types_;

    int buff_offset_;

    int cursor_;

    int available_;

    int last_useless_char_num_;

    UniquePtr<QuickSortSet> org_lexemes_;

    HashMap<int, UniquePtr<LexemePath>> path_map_;

    List<Lexeme *> results_;

    Dictionary *dict_{nullptr};

    bool ik_smart_{true};

    AnalyzeContext(Dictionary *dict, bool is_smart = true);

    int GetCursor() const { return cursor_; }

    Vector<wchar_t> GetSegmentBuff() const { return segment_buff_; }

    wchar_t GetCurrentChar() const { return segment_buff_[cursor_]; }

    int GetCurrentCharType() const { return char_types_[cursor_]; }

    int GetBufferOffset() const { return buff_offset_; }

    int FillBuffer(std::wstring &text);

    void InitCursor();

    bool MoveCursor();

    bool IsBufferConsumed() const { return cursor_ == available_ - 1; }

    void MarkBufferOffset() { buff_offset_ += cursor_; }

    bool AddLexeme(Lexeme *lexeme);

    void AddLexemePath(LexemePath *path);

    QuickSortSet *GetOrgLexemes() { return org_lexemes_.get(); }

    void OutputToResult();

    void OutputSingleCJK(int index);

    Lexeme *GetNextLexeme();

    int GetLastUselessCharNum() const { return last_useless_char_num_; }

    void Reset();

    void Compound(Lexeme *result);
};
} // namespace infinity