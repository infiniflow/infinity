export module infinity_core:ik_analyzer;

import :analyze_context;
import :lexeme;
import :arbitrator;
import :segmenter;
import :analyzer;
import :term;
import :ik_dict;
import :status;

import std;

namespace infinity {

export class IKAnalyzer : public Analyzer {
public:
    IKAnalyzer(const std::string &path);

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
    std::string dict_path_;

    bool own_dict_{};

    bool ik_smart_{true};

    Dictionary *dict_{nullptr};

    std::unique_ptr<AnalyzeContext> context_;

    std::vector<std::unique_ptr<Segmenter>> segmenters_;

    std::unique_ptr<IKArbitrator> arbitrator_;
};
} // namespace infinity