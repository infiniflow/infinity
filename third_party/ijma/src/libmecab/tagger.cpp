// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: tagger.cpp 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <cstring>
#include <iostream>

#include "viterbi.h"
#include "common.h"
#include "param.h"
#include "mecab.h"
#include "string_buffer.h"
#include "writer.h"
#include "connector.h"
#include "nbest_generator.h"
#include "stream_wrapper.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

const char *getGlobalError();
void setGlobalError(const char *str);

namespace {

const MeCab::Option long_options[] = {
  { "rcfile",        'r',  0, "FILE",    "use FILE as resource file" },
  { "dicdir",        'd',  0, "DIR",    "set DIR  as a system dicdir" },
  { "userdic",        'u',  0, "FILE",    "use FILE as a user dictionary" },
  { "lattice-level",      'l', "0", "INT",
    "lattice information level (default 0)" },
  { "dictionary-info",  'D', 0, 0, "show dictionary information and exit" },
  { "all-morphs",      'a', 0, 0,    "output all morphs(default false)" },
  { "output-format-type", 'O',  0, "TYPE",
    "set output format type (wakati,none,...)" },
  { "partial",            'p',  0, 0,      "partial parsing mode" },
  { "node-format",        'F',  "%m\\t%H\\n", "STR",
    "use STR as the user-defined node format" },
  { "unk-format",        'U',  "%m\\t%H\\n", "STR",
    "use STR as the user-defined unknown node format"   },
  { "bos-format",        'B',  "", "STR",
    "use STR as the user-defined beginning-of-sentence format"   },
  { "eos-format",        'E',  "EOS\\n", "STR",
    "use STR as the user-defined end-of-sentence format"   },
  { "eon-format",        'S',  "", "STR",
    "use STR as the user-defined end-of-NBest format"   },
  { "unk-feature",       'x',  0, "STR",
    "use STR as the feature for unknown word" },
  { "input-buffer-size",  'b',  0, "INT",
    "set input buffer size (default 8192)" },
  { "dump-config", 'P', 0, 0, "dump MeCab parameters" },
  { "open-mutable-dictionary", 'M', 0, 0,
    "open dictioanry with mutable mode (experimental)" },
  { "allocate-sentence",  'C', 0, 0,
    "allocate new memory for input sentence" },
  { "nbest",        'N', "1",    "INT",
    "output N best results (default 1)" },
  { "theta",        't',  "0.75",  "FLOAT",
    "set temparature parameter theta (default 0.75)"  },
  { "cost-factor",        'c',  "700",  "INT",
    "set cost factor (default 700)"  },
  { "output",        'o',  0,    "FILE",  "set the output file name" },
  { "version",        'v',  0, 0,     "show the version and exit." },
  { "help",          'h',  0, 0,     "show this help and exit." },
  { 0, 0, 0, 0 }
};
}

namespace MeCab {
class TaggerImpl: public Tagger {
 private:
  Tokenizer                  tokenizer_;
  Connector                  connector_;
  Viterbi                    viterbi_;
  StringBuffer               ostrs_;
  Writer                     writer_;
  scoped_ptr<NBestGenerator> nbest_;
  const char*                begin_;
  whatlog                    what_;
  long                       score; //invoke after invoking nextNode

 public:
  bool                  open(Param *);
  bool                  open(int, char**);
  bool                  open(const char*);
  void                  close();
  const char*           parse(const char*);
  const char*           parse(const char*, size_t);
  const char*           parse(const char*, size_t, char*, size_t);
  const Node*           parseToNode(const char*);
  const Node*           parseToNode(const char*, size_t = 0);
  const char*           parseNBest(size_t, const char*);
  const char*           parseNBest(size_t, const char*, size_t);
  const char*           parseNBest(size_t, const char*,
                                   size_t, char *, size_t);
  bool                  parseNBestInit(const char*);
  bool                  parseNBestInit(const char*, size_t);
  const Node*           nextNode();
  /**
   * Invoke after invoking nextNode();
   */
  long                  nextScore();
  const char*           next();
  const char*           next(char*, size_t);
  const char           *formatNode(const Node *);
  const char           *formatNode(const Node *, char *, size_t);
  const DictionaryInfo *dictionary_info() const;
  void                  set_partial(bool partial);
  bool                  partial() const;
  void                  set_theta(float theta);
  float                 theta() const;
  void                  set_lattice_level(int level);
  int                   lattice_level() const;
  void                  set_all_morphs(bool all_morphs);
  bool                  all_morphs() const;
  const char*           what();

  TaggerImpl() {}
  virtual ~TaggerImpl() { this->close(); }
};

const char *TaggerImpl::what() {
  return what_.str();
}

bool TaggerImpl::open(int argc, char **argv) {
  Param param;
  CHECK_CLOSE_FALSE(param.open(argc, argv, long_options)) << param.what();
  return open(&param);
}

bool TaggerImpl::open(const char *arg) {
  Param param;
  CHECK_CLOSE_FALSE(param.open(arg, long_options)) << param.what();
  return open(&param);
}

bool TaggerImpl::open(Param *param) {
  close();

  if (param->get<bool>("help")) {
    WHAT << param->help();
    close();
    return false;
  }

  if (param->get<bool>("version")) {
    WHAT << param->version();
    close();
    return false;
  }

  CHECK_CLOSE_FALSE(load_dictionary_resource(param)) << param->what();

  CHECK_CLOSE_FALSE(tokenizer_.open(*param)) << tokenizer_.what();
  CHECK_CLOSE_FALSE(connector_.open(*param)) << connector_.what();
  CHECK_CLOSE_FALSE(viterbi_.open(*param, &tokenizer_, &connector_))
      << viterbi_.what();
  CHECK_CLOSE_FALSE(writer_.open(*param)) << writer_.what();

  if (param->get<std::string>("output-format-type") == "dump") {
    set_lattice_level(3);
    set_all_morphs(true);
  }

#if defined(_WIN32) && !defined(__CYGWIN__)
  std::locale::global(std::locale("C"));
#endif

  return true;
}

void TaggerImpl::close() {}

void TaggerImpl::set_partial(bool partial) {
  viterbi_.set_partial(partial);
}

bool TaggerImpl::partial() const {
  return viterbi_.partial();
}

void TaggerImpl::set_theta(float theta) {
  viterbi_.set_theta(theta);
}

float TaggerImpl::theta() const {
  return viterbi_.theta();
}

void TaggerImpl::set_lattice_level(int level) {
  viterbi_.set_lattice_level(level);
}

int TaggerImpl::lattice_level() const {
  return viterbi_.lattice_level();
}

void TaggerImpl::set_all_morphs(bool all_morphs) {
  viterbi_.set_all_morphs(all_morphs);
}

bool TaggerImpl::all_morphs() const {
  return viterbi_.all_morphs();
}

const char *TaggerImpl::parse(const char *str) {
  return parse(str, std::strlen(str));
}

const char *TaggerImpl::parse(const char *str, size_t len) {
  const Node *n = parseToNode(str, len);
  if (!n) return 0;
  ostrs_.clear();
  CHECK_0(writer_.write(&ostrs_, str, n)) << writer_.what();
  ostrs_ << '\0';
  return ostrs_.str();
}

const char *TaggerImpl::parse(const char *str, size_t len,
                              char *out, size_t len2) {
  const Node *n = parseToNode(str, len);
  if (!n) return 0;
  StringBuffer os(out, len2);
  CHECK_0(writer_.write(&os, str, n)) << writer_.what();
  os << '\0';
  CHECK_0(os.str()) << "output buffer overflow";
  return os.str();
}

const char* TaggerImpl::formatNode(const Node* node) {
  ostrs_.clear();
  CHECK_0(writer_.writeNode(&ostrs_,
                            static_cast<const char *>(begin_), node))
      <<  writer_.what();
  ostrs_ << '\0';
  return ostrs_.str();
}

const char* TaggerImpl::formatNode(const Node* node,
                                   char *out, size_t len) {
  StringBuffer os(out, len);
  CHECK_0(writer_.writeNode(&os,
                            static_cast<const char *>(begin_), node))
      <<  writer_.what();
  os << '\0';
  CHECK_0(os.str()) << "output buffer overflow";
  return os.str();
}

const Node *TaggerImpl::parseToNode(const char *str) {
  return parseToNode(str, std::strlen(str));
}

const Node *TaggerImpl::parseToNode(const char *str, size_t len) {
  CHECK_RETURN(str, static_cast<Node *>(0)) << "NULL pointer is given";
  const Node *bosNode = viterbi_.analyze(str, len);
  CHECK_RETURN(bosNode, static_cast<const Node *>(0)) << viterbi_.what();
  return bosNode;
}

bool TaggerImpl::parseNBestInit(const char *str) {
  return parseNBestInit(str, std::strlen(str));
}

bool TaggerImpl::parseNBestInit(const char *str, size_t len) {
  CHECK_FALSE(viterbi_.lattice_level() >= 1) <<
      "use -l option to obtain N-Best results"
      ". e.g., mecab -N10 -l1";
  const Node *n = parseToNode(str, len);
  begin_ = str;
  if (!n) return false;
  if (!nbest_.get()) nbest_.reset(new NBestGenerator);
  nbest_->set(const_cast<Node *>(n));
  return true;
}

const Node* TaggerImpl::nextNode() {
  if (!nbest_.get()) nbest_.reset(new NBestGenerator);
  const Node *n = nbest_->next();
  CHECK_RETURN(n, static_cast<const Node*>(0)) << "no more results";
  score = n ? nbest_->nextScore() : 0;
  return n;
}

long TaggerImpl::nextScore(){
	return score;
}

const char* TaggerImpl::next() {
  const Node *n = nextNode();

  if (!n) return 0;
  ostrs_.clear();
  CHECK_0(writer_.write(&ostrs_,
                        static_cast<const char *>(begin_), n))
      << writer_.what();
  ostrs_ << '\0';
  return ostrs_.str();
}

const char* TaggerImpl::next(char *out, size_t len2) {
  const Node *n = nextNode();
  if (!n) return 0;
  StringBuffer os(out, len2);
  CHECK_0(writer_.write(&os,
                        static_cast<const char *>(begin_), n))
      << writer_.what();
  os << '\0';
  CHECK_0(os.str()) << "output buffer overflow";
  return os.str();
}

const char* TaggerImpl::parseNBest(size_t N, const char* str) {
  return parseNBest(N, str, std::strlen(str));
}

const char* TaggerImpl::parseNBest(size_t N,
                                   const char* str, size_t len) {
  if (N == 1) return parse(str, len);

  if (!parseNBestInit(str, len)) return 0;
  ostrs_.clear();

  for (size_t i = 0; i < N; ++i) {
    const Node *n = nextNode();
    if (!n) break;
    CHECK_0(writer_.write(&ostrs_, str, n)) << writer_.what();
  }

  // make a dummy node for EON
  Node eon_node;
  memset(&eon_node, 0, sizeof(eon_node));
  eon_node.stat = MECAB_EON_NODE;
  eon_node.next = 0;
  eon_node.surface = "";
  writer_.writeNode(&ostrs_, str, &eon_node);

  ostrs_ << '\0';
  return ostrs_.str();
}

const DictionaryInfo *TaggerImpl::dictionary_info() const {
  return tokenizer_.dictionary_info();
}

const char* TaggerImpl::parseNBest(size_t N, const char* str, size_t len,
                                   char *out, size_t len2) {
  if (N == 1) return parse(str, len, out, len2);

  if (!parseNBestInit(str, len)) return 0;
  StringBuffer os(out, len2);

  for (size_t i = 0; i < N; ++i) {
    const Node *n = nextNode();
    if (!n) break;
    CHECK_0(writer_.write(&os, str, n)) << writer_.what();
  }
  os << '\0';
  CHECK_0(os.str()) << "output buffer overflow";
  return os.str();
}

Tagger *Tagger::create(int argc, char **argv) {
  return createTagger(argc, argv);
}

Tagger *Tagger::create(const char *arg) {
  return createTagger(arg);
}

const char *Tagger::version() {
  return VERSION;
}

Tagger *createTagger(int argc, char **argv) {
  TaggerImpl *tagger = new TaggerImpl();
  if (!tagger->open(argc, argv)) {
    setGlobalError(tagger->what());
    delete tagger;
    return 0;
  }
  return tagger;
}

Tagger *createTagger(const char *argv) {
  TaggerImpl *tagger = new TaggerImpl();
  if (!tagger->open(argv)) {
    setGlobalError(tagger->what());
    delete tagger;
    return 0;
  }
  return tagger;
}

const char *getTaggerError() {
  return getGlobalError();
}
}

int mecab_do(int argc, char **argv) {
#define WHAT_ERROR(msg) do {                    \
    std::cout << msg << std::endl;              \
    return EXIT_FAILURE; }                      \
  while (0);

  MeCab::TaggerImpl tagger;
  MeCab::Param param;

  if (!param.open(argc, argv, long_options)) {
    std::cout << param.what() << std::endl;
    return EXIT_SUCCESS;
  }

  std::string ofilename = param.get<std::string>("output");
  if (ofilename.empty()) ofilename = "-";

  const int nbest = param.get<int>("nbest");
  if (nbest <= 0 || nbest > NBEST_MAX)
    WHAT_ERROR("invalid N value");

  if (nbest >= 2)
    param.set("lattice-level", 1, true);

  if (!tagger.open(&param)) {
    std::cout << tagger.what() << std::endl;
    std::exit(EXIT_FAILURE);
  }

  MeCab::ostream_wrapper ofs(ofilename.c_str());
  if (!*ofs)
    WHAT_ERROR("no such file or directory: " << ofilename);

  if (param.get<bool>("dump-config")) {
    param.dump_config(&*ofs);
    return EXIT_FAILURE;
  }

  if (param.get<bool>("dictionary-info")) {
    for (const MeCab::DictionaryInfo *d = tagger.dictionary_info();
         d; d = d->next) {
      *ofs << "filename:\t" << d->filename << std::endl;
      *ofs << "version:\t" << d->version << std::endl;
      *ofs << "charset:\t" << d->charset << std::endl;
      *ofs << "type:\t" << d->type   << std::endl;
      *ofs << "size:\t" << d->size << std::endl;
      *ofs << "left size:\t" << d->lsize << std::endl;
      *ofs << "right size:\t" << d->rsize << std::endl;
      *ofs << std::endl;
    }
    return EXIT_FAILURE;
  }

  const std::vector<std::string>& rest_ = param.rest_args();
  std::vector<std::string> rest = rest_;

  if (rest.empty()) rest.push_back("-");

  size_t ibufsize = MeCab::_min(MAX_INPUT_BUFFER_SIZE,
                                MeCab::_max(param.get<int>
                                            ("input-buffer-size"),
                                            MIN_INPUT_BUFFER_SIZE));

  bool partial = param.get<bool>("partial");
  if (partial) ibufsize *= 8;

  MeCab::scoped_array<char> ibuf_(new char[ibufsize]);
  char *ibuf =ibuf_.get();

  for (size_t i = 0; i < rest.size(); ++i) {
    MeCab::istream_wrapper ifs(rest[i].c_str());
    if (!*ifs) WHAT_ERROR("no such file or directory: " << rest[i]);

    while (true) {
      if (!partial) {
        ifs->getline(ibuf, ibufsize);
      } else {
        std::string sentence;
        char line[BUF_SIZE];
        for (;;) {
          if (!ifs->getline(line, sizeof(line))) {
            ifs->clear(std::ios::eofbit|std::ios::badbit);
            break;
          }
          sentence += line;
          sentence += '\n';
          if (std::strcmp(line, "EOS") == 0 || line[0] == '\0')
            break;
        }
        std::strncpy(ibuf, sentence.c_str(), ibufsize);
      }
      if (ifs->eof() && !ibuf[0]) return false;
      if (ifs->fail()) {
        std::cerr << "input-buffer overflow. "
                  << "The line is splitted. use -b #SIZE option." << std::endl;
        ifs->clear();
      }
      const char *r = (nbest >= 2) ? tagger.parseNBest(nbest, ibuf) :
          tagger.parse(ibuf);
      if (!r)  WHAT_ERROR(tagger.what());
      *ofs << r << std::flush;
    }
  }

  return EXIT_SUCCESS;

#undef WHAT_ERROR
}
