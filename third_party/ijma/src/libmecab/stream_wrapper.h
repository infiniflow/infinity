//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: stream_wrapper.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_STREAM_WRAPPER_H
#define MECAB_STREAM_WRAPPER_H

#include <iostream>
#include <fstream>
#include <cstring>

namespace MeCab {

class istream_wrapper {
 private:
  std::istream* is_;
 public:
  std::istream &operator*() const  { return *is_; }
  std::istream *operator->() const { return is_;  }
  explicit istream_wrapper(const char* filename): is_(0) {
    if (std::strcmp(filename, "-") == 0)
      is_ = &std::cin;
    else
      is_ = new std::ifstream(filename);
  }

  virtual ~istream_wrapper() {
    if (is_ != &std::cin) delete is_;
  }
};

class ostream_wrapper {
 private:
  std::ostream* os_;
 public:
  std::ostream &operator*() const  { return *os_; }
  std::ostream *operator->() const { return os_;  }
  explicit ostream_wrapper(const char* filename): os_(0) {
    if (std::strcmp(filename, "-") == 0)
      os_ = &std::cout;
    else
      os_ = new std::ofstream(filename);
  }

  virtual ~ostream_wrapper() {
    if (os_ != &std::cout) delete os_;
  }
};
}

#endif
