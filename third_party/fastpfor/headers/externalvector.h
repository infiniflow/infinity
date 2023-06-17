/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef EXTERNALVECTOR_H_
#define EXTERNALVECTOR_H_
#include <vector>
#include <queue>
#include <random>
#include "common.h"

namespace FastPForLib {

template <class CMP> class BinaryFileBuffer {
public:
  typedef uint32_t Type;
  typedef std::vector<Type> DataType;

  enum { BUFFERSIZE = 512 };
  FILE *fd;
  std::vector<DataType> buf;
  size_t currentpointer, mEnd;
  size_t localpointer;
  bool valid;
  CMP mCmp;
  size_t sizeofdata;

  BinaryFileBuffer(const BinaryFileBuffer &o)
      : fd(o.fd), buf(o.buf), currentpointer(o.currentpointer), mEnd(o.mEnd),
        localpointer(o.localpointer), valid(o.valid), mCmp(o.mCmp),
        sizeofdata(o.sizeofdata) {}
  BinaryFileBuffer()
      : fd(NULL), buf(), currentpointer(0), mEnd(0), localpointer(0),
        valid(false), mCmp(CMP()), sizeofdata(0) {}

  BinaryFileBuffer<CMP> &operator=(const BinaryFileBuffer<CMP> &o) {
    fd = o.fd;
    buf = o.buf;
    currentpointer = o.currentpointer;
    mEnd = o.mEnd;
    localpointer = o.localpointer;
    valid = o.valid;
    mCmp = o.mCmp;
    sizeofdata = o.sizeofdata;
    return *this;
  }
  BinaryFileBuffer(FILE *f, size_t start, size_t end, CMP cmp,
                   std::vector<DataType> &Buf, size_t datasize)
      : fd(f), buf(Buf), currentpointer(start), mEnd(end), localpointer(0),
        valid(true), mCmp(cmp), sizeofdata(datasize) {
    buf.reserve(BUFFERSIZE);
    reload();
  }

  void reset(size_t start) {
    currentpointer = start;
    localpointer = 0;
    valid = true;
    reload();
  }

  size_t size() { return mEnd - currentpointer; }

  void reload() {
    assert(currentpointer <= mEnd);
    size_t howmanycanIread = BUFFERSIZE;
    if (howmanycanIread > mEnd - currentpointer)
      howmanycanIread = mEnd - currentpointer;
    if (howmanycanIread == 0) {
      buf.resize(0);
      valid = false;
      return;
    }
    buf.resize(howmanycanIread);

    int result =
        fseek(fd, currentpointer * sizeofdata * sizeof(Type), SEEK_SET);
    if (result != 0) {
      std::cerr << "failed to seek" << std::endl;
      throw std::runtime_error("failed to seek");
    }
    for (size_t k = 0; k < howmanycanIread; ++k) {
      buf[k].resize(sizeofdata);
      size_t howmany = fread(&(buf[k][0]), sizeof(Type), sizeofdata, fd);
      if (howmany != sizeofdata) {
        std::cerr << " Expected to read " << sizeofdata << " but found "
                  << howmany << std::endl;
        throw std::runtime_error("failed to read the number expected");
      }
    }

    localpointer = 0;
  }

  bool hasMore() { return valid; }
  bool empty() { return !valid; }

  const DataType &peek() const { return buf[localpointer]; }

  bool operator<(const BinaryFileBuffer<CMP> &bfb) const {
    return mCmp(this->peek(), bfb.peek());
  }

  __attribute__((pure)) bool operator>(const BinaryFileBuffer<CMP> &bfb) const {
    return mCmp(bfb.peek(), this->peek());
  }
  void pop(DataType &container) {
    ++currentpointer;
    container = buf[localpointer++];
    assert(localpointer <= buf.size());
    if (localpointer == buf.size())
      reload();
  }

private:
};

class externalvector {

public:
  externalvector(size_t datasize) : fd(NULL), N(0), sizeofdata(datasize) {}
  typedef uint32_t Type;
  typedef std::vector<Type> DataType;

  ~externalvector() {}
  externalvector(const externalvector &other)
      : fd(NULL), N(0), sizeofdata(other.sizeofdata) {
    if ((other.fd != NULL) or (fd != NULL)) {
      std::cerr << "please don't use copy constructor for non-trivial things"
                << std::endl;
      throw std::runtime_error("you are abusing copy constructor");
    }
    assert(other.fd == NULL);
    assert(other.N == 0);
  }
  externalvector &operator=(const externalvector &other) {
    if ((other.fd != NULL) or (fd != NULL)) {
      std::cerr << "please don't use assignment for non-trivial things"
                << std::endl;
      throw std::runtime_error("you are abusing assignment operator");
    }
    assert(other.fd == NULL);
    assert(other.N == 0);
    fd = NULL;
    N = 0;
    sizeofdata = other.sizeofdata;
    return *this;
  }

  void swap(externalvector &o) {
    FILE *const tmpfd = fd;
    const size_t tmpN = N;
    const size_t sd = sizeofdata;
    //
    fd = o.fd;
    N = o.N;
    sizeofdata = o.sizeofdata;
    //
    o.fd = tmpfd;
    o.N = tmpN;
    o.sizeofdata = sd;
  }

  off_t getFileSize() {
    fseek(fd, 0L, SEEK_END);
    return ftell(fd);
  }

  enum { DEFAULTBLOCKSIZE = 4194304, verbose = false, vverbose = false };

  // this is not a true shuffle, it shuffles by block,
  // blocks are *not* merged. For my purposes, I did not
  // need a true shuffle.
  void shuffle(const size_t BLOCKSIZE = DEFAULTBLOCKSIZE) {
    std::vector<DataType> buffer;
    std::random_device rd;
    std::mt19937 g(rd());
    for (size_t k = 0; k < size(); k += BLOCKSIZE) {
      if (k + BLOCKSIZE < size()) {
        loadACopy(buffer, k, k + BLOCKSIZE);
        std::shuffle(buffer.begin(), buffer.end(), g);
        copyAt(buffer, k);
      } else {
        loadACopy(buffer, k, size());
        std::shuffle(buffer.begin(), buffer.end(), g);
        copyAt(buffer, k);
      }
    }
  }

  template <class CMP>
  void sort(CMP &comparator, const size_t BLOCKSIZE = DEFAULTBLOCKSIZE) {
    std::priority_queue<BinaryFileBuffer<CMP>,
                        std::vector<BinaryFileBuffer<CMP>>,
                        std::greater<BinaryFileBuffer<CMP>>> pq;

    const size_t howmanybuffers = N / BLOCKSIZE + (N % BLOCKSIZE == 0 ? 0 : 1);

    std::vector<std::vector<DataType>> buffers(howmanybuffers);
    size_t buffercounter = 0;

    std::vector<DataType> buffer;
    buffer.reserve(BLOCKSIZE);
    for (size_t rowindex = 0; rowindex < size(); rowindex += BLOCKSIZE) {

      size_t end = size();
      if (rowindex + BLOCKSIZE < size())
        end = rowindex + BLOCKSIZE;
      loadACopy(buffer, rowindex, end);
      std::sort(buffer.begin(), buffer.end(), comparator);
      copyAt(buffer, rowindex);
      BinaryFileBuffer<CMP> bfb(fd, rowindex, end, comparator,
                                buffers[buffercounter++], sizeofdata);

      pq.push(bfb);
    }
    if (buffercounter != buffers.size())
      throw std::runtime_error("This should never happen.");
    if (howmanybuffers == 1)
      return; // we are done
    // we must merge which requires a new file
    FILE *newfd = ::tmpfile();
    if (newfd == NULL) {
      throw std::runtime_error("could not open temp file");
    }

    DataType container;
    const bool bfbparanoid = false;
    DataType lastentry;
    bool first = true;

    size_t counter = 0;
    BinaryFileBuffer<CMP> bfb;
    while (!pq.empty()) {
      bfb = pq.top();
      pq.pop();

      bfb.pop(container);
      if (bfbparanoid) {
        if (!first) {
          if (comparator(container, lastentry))
            std::cout << " we have a problem at counter = " << counter
                      << std::endl;
          assert(!comparator(container, lastentry));
          lastentry = container;
        } else {
          first = false;
          lastentry = container;
        }
      }

      size_t result = fwrite(&container[0], sizeof(Type), sizeofdata, newfd);
      if (result != sizeofdata) {
        std::cerr << "Error appending to the file " << std::endl;
        break;
      }
      ++counter;
      if (!bfb.empty()) {
        pq.push(bfb); // add it back
      }
    }
    ::fclose(fd);
    fd = newfd;
  }

  bool append(const DataType &d) {
    assert(d.size() == sizeofdata);
    int results = fseek(fd, 0, SEEK_END);
    if (results != 0) {
      std::cerr << "could not seek to end of file" << std::endl;
      throw std::runtime_error("bad seek");
    }
    size_t result = fwrite(&d[0], sizeof(Type), sizeofdata, fd);
    if (result != sizeofdata) {
      std::cerr << "Error appending to the file " << std::endl;
      return false;
    }
    ++N;
    return true;
  }
  void dangerouslyfastappend(const DataType &d) {
    assert(d.size() == sizeofdata);
    fwrite(&d[0], sizeof(Type), sizeofdata, fd);
    ++N;
  }

  // caller is responsible for calling close
  void open() {
    if (vverbose)
      std::cout << "opening..." << std::endl;
    fd = tmpfile();
    if (fd == NULL) {
      std::cerr << "Can't create a temp file" << std::endl;
      throw std::runtime_error("could not open temp file");
    }
  }

  void close() {

    if (fd != NULL) {
      ::fclose(fd);
      fd = NULL;
      N = 0;
    }
  }

  void loadACopy(std::vector<DataType> &buffer, size_t begin,
                 size_t end) const {
    buffer.resize(end - begin);
    int result = fseek(fd, begin * sizeofdata * sizeof(Type), SEEK_SET);
    if (result != 0) {
      std::cerr << "could not seek to " << begin << std::endl;
      throw std::runtime_error("bad seek");
    }
    for (size_t k = 0; k < buffer.size(); ++k) {
      buffer[k].resize(sizeofdata);
      assert(buffer[k].size() == sizeofdata);
      size_t result2 = fread(&(buffer[k][0]), sizeof(Type), sizeofdata, fd);

      assert(buffer[k].size() == sizeofdata);
      if (result2 != sizeofdata) {
        std::cerr << "Error reading from file " << std::endl;
        throw std::runtime_error("bad read");
      }
    }
  }

  DataType get(const size_t pos) {

    if (fd == NULL) {
      std::cerr << "no file to read from! Open the file first!" << std::endl;
      throw std::runtime_error("file not open");
    }
    DataType ans(sizeofdata);
    int result = fseek(fd, pos * sizeofdata * sizeof(Type), SEEK_SET);
    if (result != 0) {
      std::cerr << "could not seek to " << pos << std::endl;
      throw std::runtime_error("bad seek");
    }
    size_t howmany = fread(&ans[0], sizeof(Type), sizeofdata, fd);
    if (howmany != sizeofdata) {
      std::cerr << "Error reading from file " << std::endl;
      // cerr << strerror(errno) << std::endl;
      throw std::runtime_error("bad read");
    }
    return ans;
  }
  void append(const std::vector<DataType> &buffer) {
    int result = fseek(fd, 0, SEEK_END);
    if (result != 0) {
      std::cerr << "could not seek to end of file" << std::endl;
      throw std::runtime_error("bad seek");
    }
    for (size_t k = 0; k < buffer.size(); ++k) {
      assert(buffer[k].size() == sizeofdata);
      size_t result2 = fwrite(&(buffer[k][0]), sizeof(Type), sizeofdata, fd);
      if (result2 != sizeofdata) {
        std::cerr << "Error write from file " << std::endl;
        // cerr << strerror(errno) << std::endl;
        throw std::runtime_error("bad write");
      }
    }
    N += buffer.size();
  }

  void copyAt(const std::vector<DataType> &buffer, size_t begin) {
    int result = fseek(fd, begin * sizeofdata * sizeof(Type), SEEK_SET);
    if (result != 0) {
      std::cerr << "could not seek to " << begin << std::endl;
      throw std::runtime_error("bad seek");
    }
    for (size_t k = 0; k < buffer.size(); ++k) {
      assert(buffer[k].size() == sizeofdata);
      size_t result2 = fwrite(&(buffer[k][0]), sizeof(Type), sizeofdata, fd);
      if (result2 != sizeofdata) {
        std::cerr << "Error write from file " << std::endl;
        // cerr << strerror(errno) << std::endl;
        throw std::runtime_error("bad write");
      }
    }
  }

  size_t size() const { return N; }

private:
  FILE *fd; // file descriptor
  size_t N;
  size_t sizeofdata;
};

// Lexico (basic)
class Cmp {
public:
  Cmp(std::vector<size_t> &indexes) : mIndexes(indexes) {}
  Cmp() : mIndexes() {}
  Cmp &operator=(const Cmp &v) {
    mIndexes = v.mIndexes;
    return *this;
  }

  __attribute__((pure)) bool operator()(const std::vector<uint32_t> &a,
                                        const std::vector<uint32_t> &b) const {
    for (std::vector<size_t>::const_iterator i = mIndexes.begin();
         i != mIndexes.end(); ++i) {
      const size_t k = *i;
      if (a[k] < b[k])
        return true;
      else if (a[k] > b[k])
        return false;
    }
    return false;
  }
  std::vector<size_t> mIndexes;
};

} // namespace FastPFor

#endif /* EXTERNALVECTOR_H_ */
