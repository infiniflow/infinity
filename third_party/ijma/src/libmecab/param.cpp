//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: param.cpp 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <fstream>
#include <cstdio>
#include "param.h"
#include "common.h"
#include "utils.h"
#include "string_buffer.h"

#include "jma_dictionary.h" // JMA_Dictionary
#include <strstream>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

namespace MeCab {

using namespace std;

void init_param(std::string *help,
                std::string *version,
                const std::string &system_name,
                const Option *opts) {
  *help = std::string(COPYRIGHT) + "\nUsage: " +
      system_name + " [options] files\n";

  *version = std::string(PACKAGE) + " of " + VERSION + '\n';

  size_t max = 0;
  for (size_t i = 0; opts[i].name; ++i) {
    size_t l = 1 + std::strlen(opts[i].name);
    if (opts[i].arg_description)
      l += (1 + std::strlen(opts[i].arg_description));
    max = _max(l, max);
  }

  for (size_t i = 0; opts[i].name; ++i) {
    size_t l = std::strlen(opts[i].name);
    if (opts[i].arg_description)
      l += (1 + std::strlen(opts[i].arg_description));
    *help += " -";
    *help += opts[i].short_name;
    *help += ", --";
    *help += opts[i].name;
    if (opts[i].arg_description) {
      *help += '=';
      *help += opts[i].arg_description;
    }
    for (; l <= max; l++) *help += ' ';
    *help += opts[i].description;
    *help += '\n';
  }

  *help += '\n';
  return;
}

void Param::dump_config(std::ostream *os) const {
  for (std::map<std::string, std::string>::const_iterator it = conf_.begin();
       it != conf_.end();
       ++it) {
    *os << it->first << ": " << it->second << std::endl;
  }
}

bool Param::load(const char *filename) {
  scoped_ptr<std::istream> p_ist;

  const jma::DictUnit* dict = jma::JMA_Dictionary::instance()->getDict(filename);
  if(dict)
    p_ist.reset(new std::istrstream(dict->text_, dict->length_));
  else
    p_ist.reset(new std::ifstream(filename));

  CHECK_FALSE(*p_ist) << "no such file or directory: " << filename;

  std::string line;
  while (std::getline(*p_ist, line)) {
    if (!line.size() ||
        (line.size() && (line[0] == ';' || line[0] == '#'))) continue;

    size_t pos = line.find('=');
    CHECK_FALSE(pos != std::string::npos) << "format error: " << line;

    size_t s1, s2;
    for (s1 = pos+1; s1 < line.size() && isspace(line[s1]); s1++);
    for (s2 = pos-1; static_cast<long>(s2) >= 0 && isspace(line[s2]); s2--);
    const std::string value = line.substr(s1, line.size() - s1);
    const std::string key   = line.substr(0, s2 + 1);
    set<std::string>(key.c_str(), value, false);
  }

  return true;
}

bool Param::open(int argc, char **argv, const Option *opts) {
  int ind = 0;
  int _errno = 0;

#define GOTO_ERROR(n) {                         \
    _errno = n;                                 \
    goto ERROR_LABEL; } while (0)

  if (argc <= 0) {
    system_name_ = "unknown";
    return true;  // this is not error
  }

  system_name_ = std::string(argv[0]);

  init_param(&help_, &version_, system_name_, opts);

  for (size_t i = 0; opts[i].name; ++i) {
    if (opts[i].default_value) set<std::string>
                                   (opts[i].name, opts[i].default_value);
  }

  for (ind = 1; ind < argc; ind++) {
    if (argv[ind][0] == '-') {
      // long options
      if (argv[ind][1] == '-') {
        char *s;
        for (s = &argv[ind][2]; *s != '\0' && *s != '='; s++);
        size_t len = (size_t)(s - &argv[ind][2]);
        if (len == 0) return true;  // stop the scanning

        bool hit = false;
        size_t i = 0;
        for (i = 0; opts[i].name; ++i) {
          size_t nlen = std::strlen(opts[i].name);
          if (nlen == len && std::strncmp(&argv[ind][2],
                                          opts[i].name, len) == 0) {
            hit = true;
            break;
          }
        }

        if (!hit) GOTO_ERROR(0);

        if (opts[i].arg_description) {
          if (*s == '=') {
            set<std::string>(opts[i].name, s+1);
          } else {
            if (argc == (ind+1)) GOTO_ERROR(1);
            set<std::string>(opts[i].name, argv[++ind]);
          }
        } else {
          if (*s == '=') GOTO_ERROR(2);
          set<int>(opts[i].name, 1);
        }

        // short options
      } else if (argv[ind][1] != '\0') {
        size_t i = 0;
        bool hit = false;
        for (i = 0; opts[i].name; ++i) {
          if (opts[i].short_name == argv[ind][1]) {
            hit = true;
            break;
          }
        }

        if (!hit) GOTO_ERROR(0);

        if (opts[i].arg_description) {
          if (argv[ind][2] != '\0') {
            set<std::string>(opts[i].name, &argv[ind][2]);
          } else {
            if (argc == (ind+1)) GOTO_ERROR(1);
            set<std::string>(opts[i].name, argv[++ind]);
          }
        } else {
          if (argv[ind][2] != '\0') GOTO_ERROR(2);
          set<int>(opts[i].name, 1);
        }
      }
    } else {
      rest_.push_back(std::string(argv[ind]));  // others
    }
  }

  return true;

ERROR_LABEL:
  switch (_errno) {
    case 0: WHAT << "unrecognized option `" << argv[ind] << "`"; break;
    case 1: WHAT << "`" << argv[ind] << "` requres an argument";  break;
    case 2: WHAT << "`" << argv[ind] << "` dosen't allow an argument"; break;
  }
  return false;
}

void Param::clear() {
  conf_.clear();
  rest_.clear();
}

bool Param::open(const char *arg, const Option *opts) {
  char str[BUF_SIZE];
  std::strncpy(str, arg, sizeof(str));
  char* ptr[64];
  unsigned int size = 1;
  ptr[0] = const_cast<char*>(PACKAGE);

  for (char *p = str; *p;) {
    while (isspace(*p)) *p++ = '\0';
    if (*p == '\0') break;
    ptr[size++] = p;
    if (size == sizeof(ptr)) break;
    while (*p && !isspace(*p)) p++;
  }

  return open(size, ptr, opts);
}

int Param::help_version() const {
  if (get<bool>("help")) {
    std::cout << help();
    return 0;
  }

  if (get<bool>("version")) {
    std::cout << version();
    return 0;
  }

  return 1;
}
}
