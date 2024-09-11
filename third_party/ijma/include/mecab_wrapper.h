#pragma once

#include <string>
#include <vector>

#include "mecab.h"

namespace jma {

class MeCab {
public:
    MeCab(const std::string &option);

    ~MeCab();

    bool Parse(std::vector<std::string> &out, const char *str, size_t str_len = 0);

    void SetSentence(const std::string &str);

    bool IsAlpha() const;

    bool IsSpecial() const;

    bool IsEnd() const;

    void Next();

    void GetToken(std::string &out);

    std::string GetFeature();

private:
    ::MeCab::Tagger *tagger_{nullptr};
    const ::MeCab::Node *node_{nullptr};
    char buf_[1024];
};

} // namespace jma