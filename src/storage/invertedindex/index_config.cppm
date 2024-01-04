module;

import stl;
import index_defines;
import posting_list_format;
export module index_config;

namespace infinity {
export class InvertedIndexConfig {
public:
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

    void SetAnalyzer(const String &analyzer) { analyzer_ = analyzer; }
    String GetAnalyzer() const { return analyzer_; }

private:
    String index_name_;
    PostingFormatOption posting_format_option_;
    optionflag_t flag_;
    bool is_short_list_vbyte_compress_;
    String analyzer_;
};

} // namespace infinity