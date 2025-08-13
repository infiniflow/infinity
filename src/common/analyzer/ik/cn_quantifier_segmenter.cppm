module;

export module infinity_core:cn_quantifier_segmenter;

import :stl;
import :hit;
import :segmenter;
import :analyze_context;
import :ik_dict;

import std;

namespace infinity {

export class CNQuantifierSegmenter : public Segmenter {
public:
    static const std::wstring SEGMENTER_NAME;

    static const std::wstring ChnNum;
    static HashSet<wchar_t> ChnNumberChars;
    static void InitChnNumber();

    int nstart_;
    int nend_;

    List<UniquePtr<Hit>> count_hits_;

    Dictionary *dict_{nullptr};

    CNQuantifierSegmenter(Dictionary *dict);

    void Analyze(AnalyzeContext *context) override;

    void Reset() override;

    void ProcessCNumber(AnalyzeContext *context);

    void ProcessCount(AnalyzeContext *context);

    bool NeedCountScan(AnalyzeContext *context);

    void OutputNumLexeme(AnalyzeContext *context);
};

} // namespace infinity
