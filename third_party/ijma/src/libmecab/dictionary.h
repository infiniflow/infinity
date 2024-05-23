//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: dictionary.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_DICTIONARY_H
#define MECAB_DICTIONARY_H

#include "mecab.h"
#include "darts.h"
#include "char_property.h"

namespace MeCab {

class Param;
template <class T> class Mmap;

class Dictionary {
 private:
  Mmap<char>         *dmmap_;
  const Token        *token_;
  const char         *feature_;
  const char         *charset_;
  unsigned int        version_;
  unsigned int        type_;
  unsigned int        lexsize_;
  unsigned int        lsize_;
  unsigned int        rsize_;
  std::string         filename_;
  whatlog             what_;
  Darts::DoubleArray  da_;

 public:
  typedef Darts::DoubleArray::result_pair_type result_type;

  bool open(const char *filename,
            const char *mode = "r");
  void close();

  size_t commonPrefixSearch(const char* key, size_t len,
                            result_type *result,
                            size_t rlen) {
    return da_.commonPrefixSearch(key, result, rlen, len);
  }

  result_type exactMatchSearch(const char* key) {
    result_type n;
    da_.exactMatchSearch(key, n);
    return n;
  }

  bool isCompatible(const Dictionary &d) const {
    return(version_ == d.version_ &&
           lsize_  == d.lsize_   &&
           rsize_  == d.rsize_   &&
           decode_charset(charset_) ==
           decode_charset(d.charset_));
  }

  const char *filename() const { return filename_.c_str(); }
  const char *charset() const { return const_cast<const char*>(charset_); }
  unsigned short version() const { return version_; }
  size_t  size() const { return static_cast<size_t>(lexsize_); }
  int type() const { return static_cast<int>(type_);  }
  size_t lsize() const { return static_cast<size_t>(lsize_); }
  size_t rsize() const { return static_cast<size_t>(rsize_); }

  const Token *token(const result_type &n) const {
    return token_ +(n.value >> 8);
  }
  size_t token_size(const result_type &n) const { return 0xff & n.value; }
  const char  *feature(const Token &t) const { return feature_ + t.feature; }

  static bool compile(const Param &param,
                      const std::vector<std::string> &dics,
                      const char *matrix_file,
                      const char *matrix_bin_file,
                      const char *left_id_file,
                      const char *right_id_file,
                      const char *rewrite_file,
                      const char* pos_id_file,
                      const char* output);  // outputs

  const char *what() { return what_.str(); }

  explicit Dictionary(): dmmap_(0), token_(0),
                         feature_(0), charset_(0) {}
  virtual ~Dictionary() { this->close(); }
};
}
#endif
