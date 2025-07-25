module;

export module infinity_core:segmenter;

import :analyze_context;

namespace infinity {

export class Segmenter {
public:
    virtual void Analyze(AnalyzeContext *context) = 0;

    virtual void Reset() = 0;

    virtual ~Segmenter() = default;
};

} // namespace infinity