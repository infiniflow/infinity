//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: param.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_PARAM_H
#define MECAB_PARAM_H

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include "scoped_ptr.h"
#include "common.h"

namespace {
template <class Target, class Source>
Target lexical_cast(Source arg) {
  std::stringstream interpreter;
  Target result;
  if (!(interpreter << arg) || !(interpreter >> result) ||
      !(interpreter >> std::ws).eof()) {
    MeCab::scoped_ptr<Target> r(new Target());  // return default value
    return *r;
  }
  return result;
}

template <>
std::string lexical_cast<std::string, std::string>(std::string arg) {
  return arg;
}
}

namespace MeCab {

struct Option {
  const char *name;
  char        short_name;
  const char *default_value;
  const char *arg_description;
  const char *description;
};

class Param {
 private:
  std::map<std::string, std::string> conf_;
  std::vector<std::string>           rest_;
  std::string                        system_name_;
  std::string                        help_;
  std::string                        version_;
  whatlog                            what_;

 public:
  bool open(int argc,  char **argv, const Option *opt);
  bool open(const char *arg,  const Option *opt);
  bool load(const char *filename);
  void clear();
  const std::vector<std::string>& rest_args() const { return rest_; }

  const char* program_name() const { return system_name_.c_str(); }
  const char *what() { return what_.str(); }
  const char* help() const { return help_.c_str(); }
  const char* version() const { return version_.c_str(); }
  int help_version() const;

  template <class T>
  T get(const char *key) const {
    std::map<std::string, std::string>::const_iterator it = conf_.find(key);
    if (it == conf_.end()) {
      scoped_ptr<T> r(new T());
      return *r;
    }
    return lexical_cast<T, std::string>(it->second);
  }

  template <class T>
  void set(const char* key, const T &value,
           bool rewrite = true) {
    std::string key2 = std::string(key);
    if (rewrite || (!rewrite && conf_.find(key2) == conf_.end()))
      conf_[key2] = lexical_cast<std::string, T>(value);
  }

  void dump_config(std::ostream *os) const;

  explicit Param() {}
  virtual ~Param() {}
};
}

#endif
