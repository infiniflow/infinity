//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: char_property.h 174 2009-05-30 10:15:18Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_CHARACTER_CATEGORY_H
#define MECAB_CHARACTER_CATEGORY_H

#include "ucs.h"
#include "utils.h"

namespace MeCab {
class Param;
template <class T> class Mmap;

struct CharInfo {
  unsigned int type:         18;
  unsigned int default_type: 8;
  unsigned int length:       4;
  unsigned int group:        1;
  unsigned int invoke:       1;
  bool isKindOf(CharInfo c) { return type & c.type; }
};

class CharProperty {
 private:
  MeCab::Mmap<char>         *cmmap_;
  std::vector<const char *>  clist_;
  const CharInfo            *map_;
  int                        charset_;
  whatlog                    what_;

 public:
  bool open(const Param &);
  bool open(const char*);
  void close();
  size_t size() const;
  void set_charset(const char *charset);
  int id(const char *) const;
  const char *name(size_t i) const;
  const char *what() { return what_.str(); }

  inline const char *seekToOtherType(const char *begin, const char *end,
                                     CharInfo c, CharInfo *fail,
                                     size_t *mblen, size_t *clen) const {
     const char *p =  begin;
    *clen = 0;
    while (p != end && c.isKindOf(*fail = getCharInfo(p, end, mblen))) {
      p += *mblen;
      ++(*clen);
      c = *fail;
    }
    return p;
  }

  inline CharInfo getCharInfo(const char *begin,
                              const char *end,
                              size_t *mblen) const {
    unsigned short int t = 0;
#ifndef MECAB_USE_UTF8_ONLY
    switch (charset_) {
      case EUC_JP:  t = euc_to_ucs2(begin, end, mblen); break;
      case CP932:   t = cp932_to_ucs2(begin, end, mblen); break;
      case UTF8:    t = utf8_to_ucs2(begin, end, mblen); break;
      case UTF16:   t = utf16_to_ucs2(begin, end, mblen); break;
      case UTF16LE: t = utf16le_to_ucs2(begin, end, mblen); break;
      case UTF16BE: t = utf16be_to_ucs2(begin, end, mblen); break;
      case ASCII:   t = ascii_to_ucs2(begin, end, mblen); break;
      default:      t = utf8_to_ucs2(begin, end, mblen); break;
    }
#else
    switch (charset_) {
      case UTF8:    t = utf8_to_ucs2(begin, end, mblen); break;
      case UTF16:   t = utf16_to_ucs2(begin, end, mblen); break;
      case UTF16LE: t = utf16le_to_ucs2(begin, end, mblen); break;
      case UTF16BE: t = utf16be_to_ucs2(begin, end, mblen); break;
      default:      t = utf8_to_ucs2(begin, end, mblen); break;
    }
#endif
    return map_[t];
  }

  inline CharInfo getCharInfo(size_t id) const { return map_[id]; }

  static bool compile(const char *, const char *, const char*);

  explicit CharProperty(): cmmap_(0), map_(0), charset_(0) {}
  virtual ~CharProperty() { this->close(); }
};
}
#endif
