#include "mecab_wrapper.h"
#include "mecab.h"

namespace jma {

MeCab::MeCab(const std::string &option) : tagger_(::MeCab::createTagger(option.c_str())) {}

MeCab::~MeCab() { delete tagger_; }

bool MeCab::Parse(std::vector<std::string> &out, const char *str, size_t str_len) {
    if (str_len == 0) {
        str_len = strlen(str);
    }
    const char *p = tagger_->parse(str, str_len);
    if (p == 0)
        return false;
    while (*p) {
        if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t') {
            p++;
            continue;
        }
        const char *q = strchr(p, ' ');
        if (q == 0) {
            out.push_back(p);
            break;
        }
        out.push_back(std::string(p, q));
        p = q + 1;
    }
    return true;
}

void MeCab::SetSentence(const std::string &str) { node_ = tagger_->parseToNode(str.c_str(), str.size()); }

bool MeCab::IsAlpha() const {
    const char *p = node_->feature;
    if (node_->length < 2)
        return false;
    return p[0] == 'S' && p[1] == 'L';
}

bool MeCab::IsSpecial() const {
    const char *p = node_->feature;
    return p[0] == 'S' && p[1] == 'C';
}

bool MeCab::IsEnd() const {
    if (node_ == nullptr)
        return true;
    return node_->stat == MECAB_EOS_NODE;
}

void MeCab::Next() {
    // assert(node_);
    node_ = node_->next;
}

void MeCab::GetToken(std::string &out) {
    out = node_->surface;
    strcpy(buf_, node_->surface);
    buf_[node_->length] = '\0';
    out.assign(buf_);
}

std::string MeCab::GetFeature() { return node_->feature; }

} // namespace jma
