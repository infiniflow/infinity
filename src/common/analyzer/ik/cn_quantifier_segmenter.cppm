module;

#include <string>

export module cn_quantifier_segmenter;

import stl;
import hit;
import segmenter;
import analyze_context;
import ik_dict;

namespace infinity {

export class CNQuantifierSegmenter : public Segmenter {
public:
    static const std::wstring SEGMENTER_NAME;

    static const std::wstring ChnNum;
    static HashSet<wchar_t> ChnNumberChars;
    static void initializeChnNumberChars();

    int nstart_;
    int nend_;

    List<Hit *> count_hits_;

    Dictionary *dict_;

    CNQuantifierSegmenter();

    void Analyze(AnalyzeContext *context) override;

    void Reset() override;

    void ProcessCNumber(AnalyzeContext *context);

    void ProcessCount(AnalyzeContext *context);

    bool NeedCountScan(AnalyzeContext *context);

    void OutputNumLexeme(AnalyzeContext *context);
};

} // namespace infinity
