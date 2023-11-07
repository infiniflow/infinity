module;

import stl;
import singleton;
import third_party;

export module iresearch_analyzer;

namespace infinity {

export using AnalyzerFactory = StdFunction<IResearchAnalyzer::ptr>;

export class AnalyzerPool : public Singleton<AnalyzerPool> {
public:
    using CacheType = FlatHashMap<StringView, IResearchAnalyzer::ptr>;

    IResearchAnalyzer::ptr Get(const String &name);

    void Set(const String &name, const String &args);

private:
    CacheType cache_;
};

} // namespace infinity