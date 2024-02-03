module;

import stl;
import index_defines;
import posting_list_format;
export module index_config;

namespace infinity {
export class InvertedIndexConfig {
public:
    void SetMemoryQuora(u64 quota) { memory_quota_ = quota; }

    u64 GetMemoryQuota() const { return memory_quota_; }

    void SetIndexingParallelism(u32 count) { indexing_threads_ = count; }

    u32 GetIndexingParallelism() const { return indexing_threads_; }

    void SetOptionFlag(optionflag_t flag) {
        flag_ = flag;
        posting_format_option_.InitOptionFlag(flag_);
    }

    optionflag_t GetOptionFlag() const { return flag_; }

    void SetShortListVbyteCompress(bool is_short_list_vbyte_compress) {
        is_short_list_vbyte_compress_ = is_short_list_vbyte_compress;
        posting_format_option_.SetShortListVbyteCompress(is_short_list_vbyte_compress_);
    }

    bool IsShortListVbyteCompress() const { return is_short_list_vbyte_compress_; }

    PostingFormatOption GetPostingFormatOption() const { return posting_format_option_; }

    void SetIndexName(const String &index_name) { index_name_ = index_name; }
    String GetIndexName() const { return index_name_; }

    void SetAnalyzer(const u64 column_id, const String &analyzer) { analyzers_[column_id] = analyzer; }
    String GetAnalyzer(const u64 column_id) const { return analyzers_.at(column_id); }

    void GetColumnIDs(Vector<u64> &column_ids) {
        for (auto it = analyzers_.begin(); it != analyzers_.end(); ++it) {
            column_ids.push_back(it->first);
        }
    }

private:
    String index_name_;
    u64 memory_quota_;
    u32 indexing_threads_{1};
    PostingFormatOption posting_format_option_;
    optionflag_t flag_;
    bool is_short_list_vbyte_compress_;
    String analyzer_;
    HashMap<u64, String> analyzers_;
};

} // namespace infinity