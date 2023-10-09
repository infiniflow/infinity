#pragma once

#include "doc_list_format_option.h"
#include "pos_list_format_option.h"
#include "posting_value.h"
#include "storage/invertedindex/index_config.h"
#include "storage/invertedindex/index_defines.h"

namespace infinity {

class PostingFormatOption {
public:
    inline PostingFormatOption(optionflag_t flag = OPTION_FLAG_ALL) : has_term_payload_(false) { InitOptionFlag(flag); }
    ~PostingFormatOption() = default;

    inline void Init(const std::shared_ptr<InvertedIndexConfig> &index_config) {
        InitOptionFlag(index_config->GetOptionFlag());
        doc_list_format_option_.SetShortListVbyteCompress(index_config->IsShortListVbyteCompress());
    }

    bool HasTfBitmap() const { return doc_list_format_option_.HasTfBitmap(); }

    bool HasTfList() const { return doc_list_format_option_.HasTfList(); }

    bool HasDocPayload() const { return doc_list_format_option_.HasDocPayload(); }

    bool HasPositionList() const { return pos_list_format_option_.HasPositionList(); }

    bool HasTermFrequency() const { return doc_list_format_option_.HasTF(); }

    bool HasTermPayload() const { return has_term_payload_; }

    bool IsShortListVbyteCompress() const { return doc_list_format_option_.IsShortListVbyteCompress(); }

    void SetShortListVbyteCompress(bool flag) { doc_list_format_option_.SetShortListVbyteCompress(flag); }

    const DocListFormatOption &GetDocListFormatOption() const { return doc_list_format_option_; }

    const PositionListFormatOption &GetPosListFormatOption() const { return pos_list_format_option_; }

    bool operator==(const PostingFormatOption &right) const;

    bool IsOnlyTermPayLoad() const { return HasTermPayload() && !HasTfBitmap() && !HasPositionList(); }

    PostingFormatOption GetBitmapPostingFormatOption() const;

private:
    inline void InitOptionFlag(optionflag_t flag) {
        has_term_payload_ = flag & of_term_payload;
        doc_list_format_option_.Init(flag);
        pos_list_format_option_.Init(flag);
    }

    bool has_term_payload_;
    DocListFormatOption doc_list_format_option_;
    PositionListFormatOption pos_list_format_option_;
};

class PostingFormat {
public:
    PostingFormat(const PostingFormatOption &option) : doc_list_format_(nullptr), pos_list_format_(nullptr) {
        doc_list_format_ = new DocListFormat(option.GetDocListFormatOption());
        if (option.HasPositionList()) {
            pos_list_format_ = new PositionListFormat(option.GetPosListFormatOption());
        }
    }
    ~PostingFormat() {
        if (doc_list_format_) {
            delete doc_list_format_;
            doc_list_format_ = nullptr;
        }
        if (pos_list_format_) {
            delete pos_list_format_;
            pos_list_format_ = nullptr;
        }
    }

    DocListFormat *GetDocListFormat() const { return doc_list_format_; }
    PositionListFormat *GetPositionListFormat() const { return pos_list_format_; }

private:
    DocListFormat *doc_list_format_;
    PositionListFormat *pos_list_format_;
};

} // namespace infinity