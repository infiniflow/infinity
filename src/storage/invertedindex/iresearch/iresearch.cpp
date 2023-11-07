module;

import stl;
import third_party;

module iresearch;

namespace infinity {

constexpr StringView JIEBA = "jieba";
constexpr StringView SEGMENT = "segmentation";

constexpr u64 basis = 0xCBF29CE484222325ull;
constexpr u64 prime = 0x100000001B3ull;

constexpr u64 Str2Int(char const *str, u64 last_value = basis) { return *str ? Str2Int(str + 1, (*str ^ last_value) * prime) : last_value; }

void AnalyzerPool::Set(const String &name, const String &args) {
    if (!Get(name).get()) {
        switch (Str2Int(name.c_str())) {
            case Str2Int(JIEBA.data()): {
                IResearchJiebaAnalyzer::options_t opt;
                opt.path_ = args;
                IResearchAnalyzer::ptr analyzer = MakeUnique<IResearchJiebaAnalyzer>(Move(opt));
                cache_.emplace(JIEBA, Move(analyzer));
            } break;
            case Str2Int(SEGMENT.data()): {
                IResearchSegmentationAnalyzer::options_t opt;
                IResearchAnalyzer::ptr analyzer = MakeUnique<IResearchSegmentationAnalyzer>(Move(opt));
                cache_.emplace(SEGMENT, Move(analyzer));
            } break;
            default:
                break;
        }
    }
}

IResearchAnalyzer::ptr AnalyzerPool::Get(const String &name) { return Move(cache_[name.c_str()]); }

} // namespace infinity