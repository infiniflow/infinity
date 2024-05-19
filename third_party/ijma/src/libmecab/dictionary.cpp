//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: dictionary.cpp 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#include <fstream>
#include "param.h"
#include "dictionary.h"
#include "common.h"
#include "mempool.h"
#include "dictionary_rewriter.h"
#include "connector.h"
#include "context_id.h"
#include "utils.h"
#include "iconv_utils.h"
#include "scoped_ptr.h"
#include "writer.h"
#include "mmap.h"

#include <sstream> // ostringstream, istringstream
#include <string> // string
#include "jma_dictionary.h" // JMA_UserDictionary

namespace MeCab {

static const unsigned int DictionaryMagicID = 0xef718f77u;

int progress_bar_darts(size_t current, size_t total) {
// MODIFY START - JUN
// below is commented out to disable print
//return progress_bar("emitting double-array", current, total);
    return 1;
// MODIFY END - JUN
}

bool Dictionary::open(const char *file,
                      const char *mode) {
  filename_ = file;
  MMAP_OPEN(char, dmmap_, filename_, mode);

  CHECK_CLOSE_FALSE(dmmap_->size() >= 100)
      << "dictionary file is broken: " << file;

  const char *ptr = dmmap_->begin();

  unsigned int dsize;
  unsigned int tsize;
  unsigned int fsize;
  unsigned int magic;
  unsigned int dummy;

  read_static<unsigned int>(&ptr, magic);
  CHECK_CLOSE_FALSE((magic ^ DictionaryMagicID) == dmmap_->size())
      << "dictionary file is broken: " << file;

  read_static<unsigned int>(&ptr, version_);
  CHECK_CLOSE_FALSE(version_ == DIC_VERSION)
      << "incompatible version: " << version_;

  read_static<unsigned int>(&ptr, type_);
  read_static<unsigned int>(&ptr, lexsize_);
  read_static<unsigned int>(&ptr, lsize_);
  read_static<unsigned int>(&ptr, rsize_);
  read_static<unsigned int>(&ptr, dsize);
  read_static<unsigned int>(&ptr, tsize);
  read_static<unsigned int>(&ptr, fsize);
  read_static<unsigned int>(&ptr, dummy);

  charset_ = ptr;
  ptr += 32;
  da_.set_array(reinterpret_cast<void *>(const_cast<char*>(ptr)));

  ptr += dsize;

  token_ = reinterpret_cast<const Token *>(ptr);
  ptr += tsize;

  feature_ = ptr;
  ptr += fsize;

  CHECK_CLOSE_FALSE(ptr == dmmap_->end())
      << "dictionary file is broken: " << file;

  return true;
}

void Dictionary::close() {
  MMAP_CLOSE(char, dmmap_);
}

bool Dictionary::compile(const Param &param,
                         const std::vector<std::string> &dics,
                         const char *matrix_file,
                         const char *matrix_bin_file,
                         const char *left_id_file,
                         const char *right_id_file,
                         const char *rewrite_file,
                         const char *pos_id_file,
                         const char *output) {
  Connector matrix;
  scoped_ptr<DictionaryRewriter> rewrite(0);
  scoped_ptr<POSIDGenerator> posid(0);
  scoped_ptr<ContextID> cid(0);
  scoped_ptr<Writer> writer(0);
  scoped_ptr<StringBuffer> os(0);
  Node node;

  std::vector<std::pair<std::string, Token*> > dic;

  size_t offset  = 0;
  unsigned int lexsize = 0;
  std::string w, feature, ufeature, lfeature, rfeature, fbuf, key;
  int lid, rid, cost;

  const std::string from = param.get<std::string>("dictionary-charset");
  const std::string to = param.get<std::string>("charset");
  const bool wakati = param.get<bool>("wakati");
  const int type = param.get<int>("type");
  const std::string node_format = param.get<std::string>("node-format");

  // for backward compatibility
  std::string config_charset = param.get<std::string>("config-charset");
  if (config_charset.empty()) config_charset = from;

  CHECK_DIE(!from.empty()) << "input dictionary charset is empty";
  CHECK_DIE(!to.empty())   << "output dictionary charset is empty";

  Iconv iconv;
  CHECK_DIE(iconv.open(from.c_str(), to.c_str()))
      << "iconv_open() failed with from=" << from << " to=" << to;

  Iconv config_iconv;
  CHECK_DIE(config_iconv.open(config_charset.c_str(), from.c_str()))
      << "iconv_open() failed with from=" << config_charset << " to=" << from;

  if (!node_format.empty()) {
    writer.reset(new Writer);
    os.reset(new StringBuffer);
    memset(&node, 0, sizeof(node));
  }

  if (!matrix.openText(matrix_file) &&
      !matrix.open(matrix_bin_file)) {
    matrix.set_left_size(1);
    matrix.set_right_size(1);
  }

  posid.reset(new POSIDGenerator);
  posid->open(pos_id_file, &config_iconv);

  std::istringstream iss(UNK_DEF_DEFAULT);

  jma::JMA_UserDictionary* jmaUserDictionary = jma::JMA_UserDictionary::instance();
  for (size_t i = 0; i < dics.size(); ++i) {
    std::ifstream ifs;
    std::istringstream user_iss; // for iJMA user dictionary in memory
    std::istream *is = 0;

    const char* filename = dics[i].c_str();
    const jma::DictUnit* dict = jmaUserDictionary->getDict(filename);
    if(dict) {
        user_iss.str(std::string(dict->text_, dict->length_));
        is = &user_iss;
    } else {
        ifs.open(filename);
        is = &ifs;
    }

    if (! *is) {
      if (type == MECAB_UNK_DIC) {
        std::cerr << dics[i]
                  << " is not found. minimum setting is used." << std::endl;
        is = &iss;
      } else {
        CHECK_DIE(*is) << "no such file or directory: " << dics[i];
      }
    }

// MODIFY START - JUN
// below is commented out to disable print
//std::cout << "reading " << dics[i] << " ... ";
// MODIFY END - JUN

    char line[BUF_SIZE];
    size_t num = 0;

    while (is->getline(line, sizeof(line))) {
      char *col[8];
      const size_t n = tokenizeCSV(line, col, 5);
      CHECK_DIE(n == 5) << "format error: " << line;

      w = col[0];
      lid = std::atoi(col[1]);
      rid = std::atoi(col[2]);
      cost = std::atoi(col[3]);
      feature = col[4];
      int pid = posid->id(feature.c_str());

      if (lid < 0  || rid < 0) {
        if (!rewrite.get()) {
          rewrite.reset(new DictionaryRewriter);
          rewrite->open(rewrite_file, &config_iconv);
        }

        CHECK_DIE(rewrite->rewrite(feature, &ufeature, &lfeature, &rfeature))
            << "rewrite failed: " << feature;

        if (!cid.get()) {
          cid.reset(new ContextID);
          cid->open(left_id_file, right_id_file, &config_iconv);
          CHECK_DIE(cid->left_size()  == matrix.left_size() &&
                    cid->right_size() == matrix.right_size())
              << "Context ID files("
              << left_id_file
              << " or "
              << right_id_file << " may be broken";
        }

        lid = cid->lid(lfeature.c_str());
        rid = cid->rid(rfeature.c_str());
      }

      CHECK_DIE(lid >= 0 && rid >= 0 && matrix.is_valid(lid, rid))
          << "invalid ids are found lid=" << lid << " rid=" << rid;

      if (w.empty()) {
        std::cerr << "empty word is found, discard this line" << std::endl;
        continue;
      }

      if (!iconv.convert(&feature)) {
        std::cerr << "iconv conversion failed. skip this entry"
                  << std::endl;
        continue;
      }

      if (type != MECAB_UNK_DIC && !iconv.convert(&w)) {
        std::cerr << "iconv conversion failed. skip this entry"
                  << std::endl;
        continue;
      }

      if (!node_format.empty()) {
        node.surface = w.c_str();
        node.feature = feature.c_str();
        node.length  = w.size();
        node.rlength = w.size();
        node.posid   = pid;
        node.stat    = MECAB_NOR_NODE;
        CHECK_DIE(os.get());
        CHECK_DIE(writer.get());
        os->clear();
        CHECK_DIE(writer->writeNode(&*os,
                                    node_format.c_str(),
                                    w.c_str(),
                                    &node)) <<
            "conversion error: " << feature << " with " << node_format;
        *os << '\0';
        feature = os->str();
      }

      key.clear();
      if (!wakati) key = feature + '\0';

      Token* token  = new Token;
      token->lcAttr = lid;
      token->rcAttr = rid;
      token->posid  = pid;
      token->wcost = cost;
      token->feature = offset;
      token->compound = 0;
      dic.push_back(std::pair<std::string, Token*>(w, token));

      // append to output buffer
      if (!wakati) fbuf.append(key.data(), key.size());
      offset += key.size();

      ++num;
      ++lexsize;
    }

// MODIFY START - JUN
// below is commented out to disable print
//std::cout << num << std::endl;
// MODIFY END - JUN
  }

  if (wakati) fbuf.append("\0", 1);

  std::sort(dic.begin(), dic.end());

  size_t bsize = 0;
  size_t idx = 0;
  std::string prev;
  std::vector<const char *> str;
  std::vector<size_t> len;
  std::vector<Darts::DoubleArray::result_type> val;

  for (size_t i = 0; i < dic.size(); ++i) {
    if (i != 0 && prev != dic[i].first) {
      str.push_back(dic[idx].first.c_str());
      len.push_back(dic[idx].first.size());
      val.push_back(bsize +(idx << 8));
      bsize = 1;
      idx = i;
    } else {
      ++bsize;
    }
    prev = dic[i].first;
  }
  str.push_back(dic[idx].first.c_str());
  len.push_back(dic[idx].first.size());
  val.push_back(bsize +(idx << 8));

  CHECK_DIE(str.size() == len.size());
  CHECK_DIE(str.size() == val.size());

  Darts::DoubleArray da;
  CHECK_DIE(da.build(str.size(), const_cast<char **>(&str[0]),
                     &len[0], &val[0], &progress_bar_darts) == 0)
      << "unkown error in building double-array";

  std::string tbuf;
  for (size_t i = 0; i < dic.size(); ++i) {
    tbuf.append(reinterpret_cast<const char*>(dic[i].second),
                sizeof(Token));
    delete dic[i].second;
  }
  dic.clear();

  // needs to be 8byte(64bit) aligned
  while (tbuf.size() % 8 != 0) {
    Token dummy;
    memset(&dummy, 0, sizeof(Token));
    tbuf.append(reinterpret_cast<const char*>(&dummy), sizeof(Token));
  }

  unsigned int dummy = 0;
  unsigned int lsize = matrix.left_size();
  unsigned int rsize = matrix.right_size();
  unsigned int dsize = da.unit_size() * da.size();
  unsigned int tsize = tbuf.size();
  unsigned int fsize = fbuf.size();

  unsigned int version = DIC_VERSION;
  char charset[32];
  std::fill(charset, charset + sizeof(charset), '\0');
  std::strncpy(charset, to.c_str(), 31);

  scoped_ptr<std::ostream> p_ost;
  std::ostringstream* p_strstream = 0;
  // check whether is user dict
  const jma::DictUnit* dict = jmaUserDictionary->getDict(output);
  if(dict) {
    p_strstream = new std::ostringstream(std::ios::binary|std::ios::out);
    p_ost.reset(p_strstream);
  } else
    p_ost.reset(new std::ofstream(output, std::ios::binary|std::ios::out));

  CHECK_DIE(*p_ost) << "permission denied: " << output;

  unsigned int magic = 0;

  // needs to be 64bit aligned
  // 10*32 = 64*5
  p_ost->write(reinterpret_cast<const char *>(&magic),   sizeof(unsigned int));
  p_ost->write(reinterpret_cast<const char *>(&version), sizeof(unsigned int));
  p_ost->write(reinterpret_cast<const char *>(&type),    sizeof(unsigned int));
  p_ost->write(reinterpret_cast<const char *>(&lexsize), sizeof(unsigned int));
  p_ost->write(reinterpret_cast<const char *>(&lsize),   sizeof(unsigned int));
  p_ost->write(reinterpret_cast<const char *>(&rsize),   sizeof(unsigned int));
  p_ost->write(reinterpret_cast<const char *>(&dsize),   sizeof(unsigned int));
  p_ost->write(reinterpret_cast<const char *>(&tsize),   sizeof(unsigned int));
  p_ost->write(reinterpret_cast<const char *>(&fsize),   sizeof(unsigned int));
  p_ost->write(reinterpret_cast<const char *>(&dummy),   sizeof(unsigned int));

  // 32 * 8 = 64 * 4
  p_ost->write(reinterpret_cast<const char *>(charset),  sizeof(charset));

  p_ost->write(reinterpret_cast<const char*>(da.array()),
             da.unit_size() * da.size());
  p_ost->write(const_cast<const char *>(tbuf.data()), tbuf.size());
  p_ost->write(const_cast<const char *>(fbuf.data()), fbuf.size());

  // save magic id
  magic = static_cast<unsigned int>(p_ost->tellp());
  magic ^= DictionaryMagicID;
  p_ost->seekp(0);
  p_ost->write(reinterpret_cast<const char *>(&magic), sizeof(unsigned int));

  // allocate memory for user dict, and copy its content from stream to memory
  if(dict) {
      bool result = jmaUserDictionary->copyStrToDict(p_strstream->str(), output);
      CHECK_DIE(result) << "failed to copy binary user dictionary from stream to memory.";
  }

  return true;
}
}
