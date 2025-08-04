module;

#include <fstream>

export module infinity_core:ik_analyzer;

import :stl;
import :analyze_context;
import :lexeme;
import :arbitrator;
import :segmenter;
import :analyzer;
import :term;
import :ik_dict;
import :status;

namespace infinity {

export class IKAnalyzer : public Analyzer {
public:
    IKAnalyzer(const String &path);

    IKAnalyzer(const IKAnalyzer &other);

    ~IKAnalyzer();

    Status Load();

    void SetFineGrained(bool fine_grained);

protected:
    int AnalyzeImpl(const Term &input, void *data, HookType func) override;

private:
    void Init();

    void LoadSegmenters();

    void Reset();

    int GetLastUselessCharNum();

private:
    String dict_path_;

    bool own_dict_{};

    bool ik_smart_{true};

    Dictionary *dict_{nullptr};

    UniquePtr<AnalyzeContext> context_;

    Vector<UniquePtr<Segmenter>> segmenters_;

    UniquePtr<IKArbitrator> arbitrator_;
};
} // namespace infinity