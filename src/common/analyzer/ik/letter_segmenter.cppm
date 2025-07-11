module;

#include <string>

export module infinity_core:letter_segmenter;

import :stl;
import :segmenter;
import :analyze_context;

namespace infinity {

export class LetterSegmenter : public Segmenter {
public:
    static const std::wstring SEGMENTER_NAME;
    static Vector<wchar_t> Letter_Connector;
    static Vector<wchar_t> Num_Connector;

    int start_;

    int end_;

    int english_start_;

    int english_end_;

    int arabic_start_;

    int arabic_end_;

    LetterSegmenter();

    void Analyze(AnalyzeContext *context) override;

    void Reset() override;

    bool ProcessMixLetter(AnalyzeContext *context);

    bool ProcessEnglishLetter(AnalyzeContext *context);

    bool ProcessArabicLetter(AnalyzeContext *context);

    bool IsLetterConnector(wchar_t input);

    bool IsNumConnector(wchar_t input);
};

} // namespace infinity