/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef CVSTOMAROPUCVS_H_
#define CVSTOMAROPUCVS_H_

#include <fstream>
#include <map>
#include <set>
#include <vector>
#include "common.h"

namespace FastPForLib {

enum { INCREASINGCARDINALITY, DECREASINGCARDINALITY };

/**
 * Comma-Separate Values
 */
class CSVReader {
public:
  CSVReader(std::istream *in, const std::string delimiter = ",",
            const char commentmarker = '#')
      : line(), mDelimiter(delimiter), mDelimiterPlusSpace(delimiter),
        mCommentMarker(commentmarker), mIn(in), currentData() {}
  CSVReader(const CSVReader &o)
      : line(o.line), mDelimiter(o.mDelimiter),
        mDelimiterPlusSpace(o.mDelimiterPlusSpace),
        mCommentMarker(o.mCommentMarker), mIn(o.mIn),
        currentData(o.currentData) {}
  CSVReader &operator=(const CSVReader &o) {
    line = o.line;
    mDelimiter = o.mDelimiter;
    mDelimiterPlusSpace = o.mDelimiterPlusSpace;
    mCommentMarker = o.mCommentMarker;
    mIn = o.mIn;
    currentData = o.currentData;
    return *this;
  }

  virtual ~CSVReader() {}

  void linkStream(std::istream *in) { mIn = in; }

  inline bool hasNext() {
    while (getline(*mIn, line)) {
      if (line.size() == 0)
        continue;
      if (line[0] == mCommentMarker)
        continue;
      tokenize(line);
      return true;
    }
    return false;
  }

  inline const std::vector<std::string> &nextRow() const { return currentData; }

  std::string line;

private:
  inline void tokenize(const std::string &str) {
    std::string::size_type lastPos =
        str.find_first_not_of(mDelimiterPlusSpace, 0);
    std::string::size_type pos = str.find_first_of(mDelimiter, lastPos);
    std::string::size_type pos_w = str.find_last_not_of(' ', pos);
    while (std::string::npos != pos || std::string::npos != lastPos) {
      const std::string::size_type fieldlength =
          pos == std::string::npos ? pos_w + 1 - lastPos : pos_w - lastPos;
      currentData.push_back(str.substr(lastPos, fieldlength));
      lastPos = str.find_first_not_of(mDelimiterPlusSpace, pos);
      pos = str.find_first_of(mDelimiter, lastPos);
      pos_w = str.find_last_not_of(' ', pos);
    }
  }

  std::string mDelimiter;
  std::string mDelimiterPlusSpace;
  char mCommentMarker;
  std::istream *mIn;
  std::vector<std::string> currentData;
};

class CSVFlatFile {
public:
  typedef std::map<std::string, size_t> maptype;
  typedef std::map<std::string, size_t> umaptype;

  enum { FREQNORMALISATION, DOMAINNORMALISATION };
  CSVFlatFile(const char *filename, const int normtype)
      : in(), mainreader(NULL), mapping(), NumberOfLines(0) {
    std::cout << "# computing normalization of file " << filename << std::endl;
    if (normtype == FREQNORMALISATION)
      computeFreqNormalization(filename);
    else if (normtype == DOMAINNORMALISATION)
      computeDomainNormalization(filename);
    in.open(filename);
    mainreader.linkStream(&in);
  }

  size_t getNumberOfRows() const { return NumberOfLines; }
  size_t getNumberOfColumns() const { return mapping.size(); }

  size_t numberOfAttributeValues() {
    size_t sum = 0;
    for (size_t k = 0; k < mapping.size(); ++k) {
      sum += mapping[k].size();
    }
    return sum;
  }

  std::vector<size_t> cardinalities() {
    std::vector<size_t> cardinalities;
    for (size_t k = 0; k < mapping.size(); ++k) {
      cardinalities.push_back(mapping[k].size());
    }
    return cardinalities;
  }

  std::vector<size_t>
  computeColumnOrderAndReturnColumnIndexes(int order = INCREASINGCARDINALITY) {
    std::vector<size_t> cardinalities;
    for (size_t k = 0; k < mapping.size(); ++k) {
      cardinalities.push_back(mapping[k].size());
    }
    std::vector<std::pair<size_t, size_t>> cardinalitiesindex;
    for (size_t k = 0; k < cardinalities.size(); ++k)
      cardinalitiesindex.push_back(
          std::pair<size_t, size_t>(cardinalities[k], k));
    if (order == INCREASINGCARDINALITY)
      sort(cardinalitiesindex.begin(), cardinalitiesindex.end());
    else
      sort(cardinalitiesindex.rbegin(), cardinalitiesindex.rend());
    std::vector<size_t> answer(getNumberOfColumns());
    for (size_t k = 0; k < answer.size(); ++k)
      answer[k] = cardinalitiesindex[k].second;
    return answer;
  }

  // a bit awkward... but convenient:
  std::set<size_t>
  AllButTopCardinalityColumnsExceptThoseWithCardinalityLowerThan(
      size_t topcolumns, size_t threshold) {
    if (mapping.empty()) {
      std::set<size_t> answer;
      return answer;
    }
    std::vector<size_t> allcards;
    for (size_t k = 0; k < mapping.size(); ++k) {
      allcards.push_back(mapping[k].size());
    }
    sort(allcards.rbegin(), allcards.rend());
    size_t mythreshold =
        allcards[std::min(topcolumns - 1, allcards.size() - 1)];
    std::cout << "# top " << topcolumns << " columns selected with cardinality "
              << mythreshold << std::endl;
    if (mythreshold < threshold)
      mythreshold = threshold;
    std::cout << "# cardinality threshold " << mythreshold << std::endl;
    std::set<size_t> answer;
    for (size_t k = 0; k < mapping.size(); ++k) {
      if (mapping[k].size() < mythreshold) {
        answer.insert(k);
      }
    }
    return answer;
  }
  void clear() { mapping.clear(); }
  void close() { in.close(); }

  // size_t getCardinalityOfColumn(size_t k) {return mapping[k].size();}
  template <class C> bool nextRow(C &container) {
    if (mainreader.hasNext()) {
      const std::vector<std::string> &row = mainreader.nextRow();
      for (size_t k = 0; k < row.size(); ++k) {
        container[k] = static_cast<uint32_t>(mapping[k][row[k]]);
      }
      return true;
    } else
      return false;
  }

  void computeHisto(std::ifstream &fsin, std::vector<umaptype> &histograms) {
    NumberOfLines = 0;
    CSVReader csvfile(&fsin);
    if (csvfile.hasNext()) {
      ++NumberOfLines;
      const std::vector<std::string> &row = csvfile.nextRow();
      histograms.resize(row.size());
      for (size_t k = 0; k < row.size(); ++k) {
        histograms[k][row[k]] = 1;
      }
    } else {
      std::cerr << "could open the file, but couldn't even read the first line"
                << std::endl;
      return;
    }
    while (csvfile.hasNext()) {
      ++NumberOfLines;
      const std::vector<std::string> &row = csvfile.nextRow();
      for (size_t k = 0; k < row.size(); ++k) {
        histograms[k][row[k]] += 1;
      }
    }
  }
  // map the string values to integer per frequency
  void computeFreqNormalization(const char *filename) {
    std::ifstream fsin(filename);
    if (!fsin) {
      std::cerr << "can't open " << filename << std::endl;
      return;
    }

    std::vector<umaptype> histograms;
    computeHisto(fsin, histograms);

    fsin.close();
    mapping.resize(histograms.size());
    // next we sort the values per frequency
    for (size_t k = 0; k < histograms.size(); ++k) {
      std::vector<std::pair<size_t, std::string>> tobesorted;
      tobesorted.reserve(histograms[k].size());
      for (umaptype::iterator i = histograms[k].begin();
           i != histograms[k].end(); ++i) {
        tobesorted.push_back(
            std::pair<size_t, std::string>(i->second, i->first));
      }
      histograms[k].clear(); // free the memory
      sort(tobesorted.rbegin(), tobesorted.rend());
      maptype &thismap = mapping[k];
      for (size_t k = 0; k < tobesorted.size(); ++k)
        thismap[tobesorted[k].second] = k;
    }
  }

  // map the string values to integers in lexicographical order
  void computeDomainNormalization(const char *filename) {
    std::ifstream fsin(filename);
    if (!fsin) {
      std::cerr << "can't open " << filename << std::endl;
      return;
    }
    NumberOfLines = 0;
    CSVReader csvfile(&fsin);
    std::vector<std::set<std::string>> histograms;
    if (csvfile.hasNext()) {
      ++NumberOfLines;
      const std::vector<std::string> &row = csvfile.nextRow();
      histograms.resize(row.size());
      for (size_t k = 0; k < row.size(); ++k) {
        histograms[k].insert(row[k]);
      }
    } else {
      std::cerr
          << "could open the file, but couldn't even read the first line of "
          << filename << std::endl;
      return;
    }
    while (csvfile.hasNext()) {
      ++NumberOfLines;
      const std::vector<std::string> &row = csvfile.nextRow();
      for (size_t k = 0; k < row.size(); ++k) {
        histograms[k].insert(row[k]);
      }
    }
    fsin.close();
    mapping.resize(histograms.size());
    // the values are already sorted lexicographically
    for (size_t k = 0; k < histograms.size(); ++k) {
      std::set<std::string> &myvalues = histograms[k];
      maptype &thismap = mapping[k];
      size_t counter = 0;
      for (std::set<std::string>::iterator i = myvalues.begin();
           i != myvalues.end(); ++i) {
        thismap[*i] = counter++;
      }
      myvalues.clear();
    }
  }

  std::ifstream in;
  CSVReader mainreader;
  std::vector<maptype> mapping;
  size_t NumberOfLines;
};

} // namespace FastPFor

#endif /* CVSTOMAROPU_H_ */
