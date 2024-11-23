module;

#include <string>

export module analyze_context;

import stl;
import quick_sort_set;
import character_util;
import lexeme;
import lexeme_path;
import ik_dict;

namespace infinity {

export class AnalyzeContext {
public:
    static const int BUFF_SIZE = 4096;

    static const int BUFF_EXHAUST_CRITICAL = 100;

    Vector<wchar_t> segment_buff_;

    Vector<int> char_types_;

    int buff_offset_;

    int cursor_;

    int available_;

    int last_useless_char_num_;

    HashSet<std::wstring> buff_locker_;

    QuickSortSet org_lexemes_;

    HashMap<int, LexemePath *> path_map_;

    List<Lexeme *> results_;

    Dictionary *dict_;

    AnalyzeContext();

    int GetCursor() const { return cursor_; }

    Vector<wchar_t> GetSegmentBuff() const { return segment_buff_; }

    wchar_t GetCurrentChar() const { return segment_buff_[cursor_]; }

    int GetCurrentCharType() const { return char_types_[cursor_]; }

    int GetBufferOffset() const { return buff_offset_; }

    int FillBuffer(std::wifstream &reader);

    void InitCursor();

    bool MoveCursor();

    void LockBuffer(const std::wstring &segmenterName) { buff_locker_.insert(segmenterName); }

    void UnlockBuffer(const std::wstring &segmenterName) { buff_locker_.erase(segmenterName); }

    bool IsBufferLocked() const { return !buff_locker_.empty(); }

    bool IsBufferConsumed() const { return cursor_ == available_ - 1; }

    bool NeedRefillBuffer() const;

    void MarkBufferOffset() { buff_offset_ += cursor_; }

    void AddLexeme(Lexeme *lexeme);

    void AddLexemePath(LexemePath *path);

    QuickSortSet *GetOrgLexemes() { return &(org_lexemes_); }

    void OutputToResult();

    void OutputSingleCJK(int index);

    Lexeme *GetNextLexeme();

    int GetLastUselessCharNum() const { return last_useless_char_num_; }

    void Reset();

    void Compound(Lexeme *result);
};
} // namespace infinity