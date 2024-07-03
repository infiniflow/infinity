module;

export module posting_list_format;

import stl;
import doc_list_format_option;
import position_list_format_option;
import posting_field;
import index_defines;

namespace infinity {

export class PostingFormatOption {
public:
    inline PostingFormatOption(optionflag_t flag = OPTION_FLAG_ALL)
        : has_term_payload_(flag & of_term_payload), doc_list_format_option_(flag), pos_list_format_option_(flag) {}

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

    bool IsOnlyTermPayLoad() const { return HasTermPayload() && !HasPositionList(); }

private:
    bool has_term_payload_;
    DocListFormatOption doc_list_format_option_;
    PositionListFormatOption pos_list_format_option_;
};

export class PostingFormat {
public:
    explicit PostingFormat(const PostingFormatOption &option) : option_(option), doc_list_format_(nullptr), pos_list_format_(nullptr) {
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
    const PostingFormatOption GetOption() const { return option_; }

private:
    const PostingFormatOption option_;

private:
    DocListFormat *doc_list_format_;
    PositionListFormat *pos_list_format_;
};

} // namespace infinity