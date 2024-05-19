//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//  $Id: dictionary_compiler.cpp 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "mecab.h"
#include "dictionary_rewriter.h"
#include "char_property.h"
#include "param.h"
#include "connector.h"
#include "dictionary.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

namespace MeCab {

class DictionaryComplier {
 public:
  static int run(int argc, char **argv) {
    static const MeCab::Option long_options[] = {
      { "dicdir",   'd',   ".",   "DIR", "set DIR as dicdi (default \".\")" },
      { "outdir",   'o',   ".",   "DIR",
        "set DIR as output dir (default \".\")" },
      { "unknown",  'U',   0,   0,   "build parameters for unknown words" },
      { "userdic",  'u',   0,   "FILE",   "build user dictionary" },
      { "charcategory", 'C', 0, 0,   "build character category maps" },
      { "matrix",    'm',  0,   0,   "build connection matrix" },
      { "charset",   'c',  MECAB_DEFAULT_CHARSET, "ENC",
        "make charset of binary dictionary ENC (default "
        MECAB_DEFAULT_CHARSET ")"  },
      { "charset",   't',  MECAB_DEFAULT_CHARSET, "ENC", "alias of -c"  },
// MODIFY START - JUN
// to enable the "dictionary-charset" entry in "dicrc" file, set the default value to empty below.
      //{ "dictionary-charset",  'f',  MECAB_DEFAULT_CHARSET,
      { "dictionary-charset",  'f',  0,
// MODIFY END - JUN
        "ENC", "assume charset of input CSVs as ENC (default "
        MECAB_DEFAULT_CHARSET ")"  },
      { "wakati",    'w',  0,   0,   "build wakati-gaki only dictionary", },
      { "posid",     'p',  0,   0,   "assign Part-of-speech id" },
      { "node-format", 'F', 0,  "STR",
        "use STR as the user defined node format" },
      { "version",   'v',  0,   0,   "show the version and exit."  },
      { "help",      'h',  0,   0,   "show this help and exit."  },
      { 0, 0, 0, 0 }
    };

    Param param;

    if (!param.open(argc, argv, long_options)) {
      std::cout << param.what() << "\n\n" <<  COPYRIGHT
                << "\ntry '--help' for more information." << std::endl;
      return -1;
    }

    if (!param.help_version()) return 0;

    const std::string dicdir = param.get<std::string>("dicdir");
    const std::string outdir = param.get<std::string>("outdir");
    bool opt_unknown = param.get<bool>("unknown");
    bool opt_matrix = param.get<bool>("matrix");
    bool opt_charcategory = param.get<bool>("charcategory");
    bool opt_sysdic = param.get<bool>("sysdic");
    const std::string userdic = param.get<std::string>("userdic");

#define DCONF(file) create_filename(dicdir, std::string(file)).c_str()
#define OCONF(file) create_filename(outdir, std::string(file)).c_str()

    CHECK_DIE(param.load(DCONF(DICRC)))
        << "no such file or directory: " << DCONF(DICRC);

// ADD START - JUN
// if the "dictionary-charset" entry is not defined in "dicrc" file or command option,
// it would be reset to "MECAB_DEFAULT_CHARSET" before dictionary compiling.
    if(param.get<std::string>("dictionary-charset").empty())
    {
        param.set<std::string>("dictionary-charset", MECAB_DEFAULT_CHARSET);
    }
// ADD END - JUN

    std::vector<std::string> dic;
    if (userdic.empty())
      enum_csv_dictionaries(dicdir.c_str(), &dic);
    else
      dic = param.rest_args();

    if (!userdic.empty()) {
      CHECK_DIE(dic.size()) << "no dictionaries are specified";

      param.set("type", MECAB_USR_DIC);
      Dictionary::compile(param, dic,
                          DCONF(MATRIX_DEF_FILE),
                          DCONF(MATRIX_FILE),
                          DCONF(LEFT_ID_FILE),
                          DCONF(RIGHT_ID_FILE),
                          DCONF(REWRITE_FILE),
                          DCONF(POS_ID_FILE),
                          userdic.c_str());
    } else {
      if (!opt_unknown && !opt_matrix && !opt_charcategory && !opt_sysdic) {
        opt_unknown = opt_matrix = opt_charcategory = opt_sysdic = true;
      }

      if (opt_charcategory || opt_unknown) {
        CharProperty::compile(DCONF(CHAR_PROPERTY_DEF_FILE),
                              DCONF(UNK_DEF_FILE),
                              OCONF(CHAR_PROPERTY_FILE));
      }

      if (opt_unknown) {
        std::vector<std::string> tmp;
        tmp.push_back(DCONF(UNK_DEF_FILE));
        param.set("type", MECAB_UNK_DIC);
        Dictionary::compile(param, tmp,
                            DCONF(MATRIX_DEF_FILE),
                            DCONF(MATRIX_FILE),
                            DCONF(LEFT_ID_FILE),
                            DCONF(RIGHT_ID_FILE),
                            DCONF(REWRITE_FILE),
                            DCONF(POS_ID_FILE),
                            OCONF(UNK_DIC_FILE));
      }

      if (opt_sysdic) {
        CHECK_DIE(dic.size()) << "no dictionaries are specified";
        param.set("type", MECAB_SYS_DIC);
        Dictionary::compile(param, dic,
                            DCONF(MATRIX_DEF_FILE),
                            DCONF(MATRIX_FILE),
                            DCONF(LEFT_ID_FILE),
                            DCONF(RIGHT_ID_FILE),
                            DCONF(REWRITE_FILE),
                            DCONF(POS_ID_FILE),
                            OCONF(SYS_DIC_FILE));
      }

      if (opt_matrix) {
        Connector::compile(DCONF(MATRIX_DEF_FILE),
                           OCONF(MATRIX_FILE));
      }
    }

// MODIFY START - JUN
// below is commented out to disable print
//std::cout << "\ndone!\n";
// MODIFY END - JUN

    return 0;
  }
};

#undef DCONF
#undef OCONF
}

int mecab_dict_index(int argc, char **argv) {
  return MeCab::DictionaryComplier::run(argc, argv);
}
