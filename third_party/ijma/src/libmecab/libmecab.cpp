//
//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: libmecab.cpp 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#endif

#include "mecab.h"
#include "mutex.h"
#include "utils.h"
#include "tokenizer.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

static const int LIBMECAB_ID = 77718;

MeCab::Mutex *getGlobalMutex() {
  static MeCab::Mutex m;
  return &m;
}

std::string *getStaticErrorString() {
  static std::string errorStr;
  return &errorStr;
}

const char *getGlobalError() {
  return getStaticErrorString()->c_str();
}

void setGlobalError(const char *str) {
  MeCab::Mutex *m = getGlobalMutex();
  m->lock();
  std::string *error = getStaticErrorString();
  *error = str;
  m->unlock();
}

struct mecab_t {
  int allocated;
  MeCab::Tagger* ptr;
};

#if defined(_WIN32) && !defined(__CYGWIN__)
HINSTANCE DllInstance = 0;

#ifdef __cplusplus
extern "C" {
#endif
  BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID) {
    if (!DllInstance) {
      DllInstance = hinst;
    }
    std::locale loc(std::locale("japanese"),
                    "C", std::locale::numeric);
    std::locale::global(loc);
    return TRUE;
  }
#ifdef __cplusplus
}
#endif
#endif

mecab_t* mecab_new(int argc, char **argv) {
  mecab_t *c = new mecab_t;
  MeCab::Tagger *ptr = MeCab::createTagger(argc, argv);
  if (!c || !ptr) {
    delete c;
    delete ptr;
    setGlobalError(MeCab::getTaggerError());
    return 0;
  }
  c->ptr = ptr;
  c->allocated = LIBMECAB_ID;
  return c;
}

mecab_t* mecab_new2(const char *arg) {
  mecab_t *c = new mecab_t;
  MeCab::Tagger *ptr = MeCab::createTagger(arg);
  if (!c || !ptr) {
    delete c;
    delete ptr;
    setGlobalError(MeCab::getTaggerError());
    return 0;
  }
  c->ptr = ptr;
  c->allocated = LIBMECAB_ID;
  return c;
}

const char *mecab_version() {
  return MeCab::Tagger::version();
}

const char* mecab_strerror(mecab_t *c) {
  if (!c || !c->allocated)
    return const_cast<char *>(getGlobalError());
  return c->ptr->what();
}

void mecab_destroy(mecab_t *c) {
  if (c && c->allocated) {
    delete c->ptr;
    delete c;
  }
  c = 0;
}

#define MECAB_CHECK_FIRST_ARG(c, t)                     \
  if (!(c) || (c)->allocated != LIBMECAB_ID) {          \
    setGlobalError("first argment seems invalid");      \
    return 0;                                           \
  } MeCab::Tagger *(t) = (c)->ptr;

#define MECAB_CHECK_FIRST_ARG_VOID(c, t)                \
  if (!(c) || (c)->allocated != LIBMECAB_ID) {          \
    setGlobalError("first argment seems invalid");      \
    return;                                             \
  } MeCab::Tagger *(t) = (c)->ptr;

int  mecab_get_partial(mecab_t *c) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->partial();
}

void mecab_set_partial(mecab_t *c, int partial) {
  MECAB_CHECK_FIRST_ARG_VOID(c, t);
  t->set_partial(partial);
}

float  mecab_get_theta(mecab_t *c) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->theta();
}

void mecab_set_theta(mecab_t *c, float theta) {
  MECAB_CHECK_FIRST_ARG_VOID(c, t);
  t->set_theta(theta);
}

int  mecab_get_lattice_level(mecab_t *c) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->lattice_level();
}

void mecab_set_lattice_level(mecab_t *c, int level) {
  MECAB_CHECK_FIRST_ARG_VOID(c, t);
  t->set_lattice_level(level);
}

int mecab_get_all_morphs(mecab_t *c) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return static_cast<int>(t->all_morphs());
}

void mecab_set_all_morphs(mecab_t *c, int all_morphs) {
  MECAB_CHECK_FIRST_ARG_VOID(c, t);
  t->set_all_morphs(all_morphs);
}

const char* mecab_sparse_tostr(mecab_t *c, const char *str) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->parse(str);
}

const char* mecab_sparse_tostr2(mecab_t *c, const char *str, size_t len) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->parse(str, len);
}

char* mecab_sparse_tostr3(mecab_t *c, const char *str, size_t len,
                          char *out, size_t len2) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return const_cast<char *>(t->parse(str, len, out, len2));
}

const mecab_node_t* mecab_sparse_tonode(mecab_t *c, const char *str) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return reinterpret_cast<const mecab_node_t *>(t->parseToNode(str));
}

const mecab_node_t* mecab_sparse_tonode2(mecab_t *c,
                                         const char *str, size_t len) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return reinterpret_cast<const mecab_node_t *>(t->parseToNode(str, len));
}

const char* mecab_nbest_sparse_tostr(mecab_t* c, size_t N,
                                     const char *str) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->parseNBest(N, str);
}

const char* mecab_nbest_sparse_tostr2(mecab_t* c, size_t N,
                                      const char* str, size_t len) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->parseNBest(N, str, len);
}

char* mecab_nbest_sparse_tostr3(mecab_t* c, size_t N,
                                const char *str, size_t len,
                                char *out, size_t len2) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return const_cast<char *>(t->parseNBest(N, str, len, out, len2));
}

int mecab_nbest_init(mecab_t *c, const char *str) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->parseNBestInit(str);
}

int mecab_nbest_init2(mecab_t *c, const char *str, size_t len) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->parseNBestInit(str, len);
}

const char* mecab_nbest_next_tostr(mecab_t* c) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->next();
}

char* mecab_nbest_next_tostr2(mecab_t* c, char *out, size_t len2) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return const_cast<char *>(t->next(out, len2));
}

const mecab_node_t* mecab_nbest_next_tonode(mecab_t* c) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return reinterpret_cast<const mecab_node_t *>(t->nextNode());
}

const char* mecab_format_node(mecab_t *c, const mecab_node_t* n) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return t->formatNode(n);
}

const mecab_dictionary_info_t *mecab_dictionary_info(mecab_t *c) {
  MECAB_CHECK_FIRST_ARG(c, t);
  return(const mecab_dictionary_info_t *)(t->dictionary_info());
}

