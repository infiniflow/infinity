module;

#include <fstream>

export module ik_segmenter;

import stl;
import analyze_context;
import lexeme;
import arbitrator;
import segmenter;
import analyzer;
import term;
import ik_dict;
import status;

namespace infinity {

class IKSegmenter : public Analyzer {
private:
    String dict_path_;

    bool own_dict_{};

    Dictionary *dict_{nullptr};

    SharedPtr<AnalyzeContext> context_;

    Vector<SharedPtr<Segmenter>> segmenters_;

    SharedPtr<IKArbitrator> arbitrator_;

public:
    IKSegmenter(const String &path);

    IKSegmenter(const IKSegmenter &other);

    ~IKSegmenter();

    Status Load();

    Vector<SharedPtr<Segmenter>> LoadSegmenters();

    Lexeme *Next();

    void Reset();

    int GetLastUselessCharNum();

protected:
    int AnalyzeImpl(const Term &input, void *data, HookType func) override;
};
} // namespace infinity