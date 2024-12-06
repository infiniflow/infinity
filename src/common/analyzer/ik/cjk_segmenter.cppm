module;

#include <string>

export module cjk_segmenter;

import stl;
import hit;
import segmenter;
import analyze_context;
import ik_dict;

namespace infinity {

export class CJKSegmenter : public Segmenter {
public:
    static const std::wstring SEGMENTER_NAME;

    List<UniquePtr<Hit>> tmp_hits_;

    Dictionary *dict_{nullptr};

    CJKSegmenter(Dictionary *dict);

    void Analyze(AnalyzeContext *context) override;

    void Reset() override;
};

} // namespace infinity