module;

import index_defines;
export module index_config;

namespace infinity {
export class InvertedIndexConfig {
public:
    void SetOptionFlag(optionflag_t flag) { flag_ = flag; }
    optionflag_t GetOptionFlag() const { return flag_; }
    void SetShortListVbyteCompress(bool is_short_list_vbyte_compress) { is_short_list_vbyte_compress_ = is_short_list_vbyte_compress; }
    bool IsShortListVbyteCompress() const { return is_short_list_vbyte_compress_; }

private:
    optionflag_t flag_;
    bool is_short_list_vbyte_compress_;
};

} // namespace infinity