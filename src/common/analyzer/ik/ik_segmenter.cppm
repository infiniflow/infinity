module;

#include <fstream>

export module ik_segmenter;

import stl;
import analyze_context;
import lexeme;
import arbitrator;
import segmenter;

namespace infinity {

class IKSegmenter {
private:
    SharedPtr<AnalyzeContext> context_;
    Vector<SharedPtr<Segmenter>> segmenters_;
    SharedPtr<IKArbitrator> arbitrator_;
    std::wifstream input_;

public:
    IKSegmenter();

    void Init();

    Vector<SharedPtr<Segmenter>> LoadSegmenters();

    Lexeme *Next();

    void Reset();

    int GetLastUselessCharNum();
};
} // namespace infinity