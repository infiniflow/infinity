// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: mmap.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_MMAP_H
#define MECAB_MMAP_H

#include <errno.h>
#include <string>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <fstream>
#endif

#include "jma_dictionary.h" // DictUnit

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#else

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif
}

#include "common.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

#if !defined(_WIN32) || defined(__CYGWIN__)
namespace {
int open__(const char* name, int flag) { return open(name, flag); }
int close__(int fd) { return close(fd); }
}
#endif

namespace MeCab {

template <class T> class Mmap {
 private:
  T            *text;
  size_t       length;
  std::string  fileName;
  whatlog what_;

#if defined(_WIN32) && !defined(__CYGWIN__)
  HANDLE hFile;
  HANDLE hMap;
#else
  int    fd;
  int    flag;
#endif

  // for jma dictionary archive
  bool isArchiveSection;

 public:
  T&       operator[](size_t n)       { return *(text + n); }
  const T& operator[](size_t n) const { return *(text + n); }
  T*       begin()           { return text; }
  const T* begin()    const  { return text; }
  T*       end()           { return text + size(); }
  const T* end()    const  { return text + size(); }
  size_t size()               { return length/sizeof(T); }
  const char *what()          { return what_.str(); }
  const char *file_name()     { return fileName.c_str(); }
  size_t file_size()          { return length; }
  bool empty()                { return(length == 0); }

  bool isArchive() const { return isArchiveSection; }
  Mmap(const jma::DictUnit& dict): text(reinterpret_cast<T *>(dict.text_)), length(dict.length_), fileName(dict.fileName_), isArchiveSection(true) {}

  // This code is imported from sufary, develoved by
  //  TATUO Yamashita <yto@nais.to> Thanks!
#if defined(_WIN32) && !defined(__CYGWIN__)
  bool open(const char *filename, const char *mode = "r") {
    this->close();

    unsigned long mode1, mode2, mode3;
    fileName = std::string(filename);

    if (std::strcmp(mode, "r") == 0) {
        mode1 = GENERIC_READ;
        mode2 = PAGE_READONLY;
        mode3 = FILE_MAP_READ;
    } else if (std::strcmp(mode, "r+") == 0) {
        mode1 = GENERIC_READ | GENERIC_WRITE;
        mode2 = PAGE_READWRITE;
        mode3 = FILE_MAP_ALL_ACCESS;
    } else {
        CHECK_CLOSE_FALSE(false) << "unknown open mode:" << filename;
    }

    hFile = CreateFile(filename, mode1, FILE_SHARE_READ, 0,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    CHECK_CLOSE_FALSE(hFile != INVALID_HANDLE_VALUE)
        << "CreateFile() failed: " << filename;

    length = GetFileSize(hFile, 0);

    hMap = CreateFileMapping(hFile, 0, mode2, 0, 0, 0);
    CHECK_CLOSE_FALSE(hMap) << "CreateFileMapping() failed: " << filename;

    text = reinterpret_cast<T *>(MapViewOfFile(hMap, mode3, 0, 0, 0));
    CHECK_CLOSE_FALSE(text) << "MapViewOfFile() failed: " << filename;

    // allocate memory directly instead of mmap
    /*std::ifstream ifs;
    ifs.open(filename, std::ios::binary);
    CHECK_CLOSE_FALSE(ifs) << "open failed: " << filename;

    // get length of file
    ifs.seekg(0, std::ios::end);
    length = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    // allocate memory
    text = new T[length];
    CHECK_CLOSE_FALSE(text) << "new[" << length << "] failed: " << filename;

    // read data as a block
    ifs.read(reinterpret_cast<char *>(text),length);
    CHECK_CLOSE_FALSE(ifs.gcount() == length) << "read() failed: " << filename;
    ifs.close();*/

    return true;
  }

  void close() {
    // check jma dictionary archive
    if (isArchiveSection) {
        text = 0;
        isArchiveSection = false;
        return;
    }

    if (text) { UnmapViewOfFile(text); }
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
    if (hMap) {
        CloseHandle(hMap);
        hMap = 0;
    }
    text = 0;

    // free memory directly instead of mmap
    /*if (text) {
        delete [] text;
        text = 0;
    }*/
  }

  Mmap(): text(0), hFile(INVALID_HANDLE_VALUE), hMap(0), isArchiveSection(false) {}

#else

  bool open(const char *filename, const char *mode = "r") {
    this->close();

    struct stat st;
    fileName = std::string(filename);

    if      (std::strcmp(mode, "r") == 0)
      flag = O_RDONLY;
    else if (std::strcmp(mode, "r+") == 0)
      flag = O_RDWR;
    else
      CHECK_CLOSE_FALSE(false) << "unknown open mode: " << filename;

    CHECK_CLOSE_FALSE((fd = open__(filename, flag | O_BINARY)) >= 0)
        << "open failed: " << filename;

    CHECK_CLOSE_FALSE(fstat(fd, &st) >= 0)
        << "failed to get file size: " << filename;

    length = st.st_size;

#ifdef HAVE_MMAP
    int prot = PROT_READ;
    if (flag == O_RDWR) prot |= PROT_WRITE;
    char *p;
    CHECK_CLOSE_FALSE((p = reinterpret_cast<char *>
                       (mmap(0, length, prot, MAP_SHARED, fd, 0)))
                      != MAP_FAILED)
        << "mmap() failed: " << filename;

    text = reinterpret_cast<T *>(p);
#else
    text = new T[length];
    CHECK_CLOSE_FALSE(read(fd, text, length) >= 0)
        << "read() failed: " << filename;
#endif
    close__(fd);
    fd = -1;

    return true;
  }

  void close() {
    // check jma dictionary archive
    if (isArchiveSection) {
        text = 0;
        isArchiveSection = false;
        return;
    }

    if (fd >= 0) {
      close__(fd);
      fd = -1;
    }

    if (text) {
#ifdef HAVE_MMAP
      munmap(reinterpret_cast<char *>(text), length);
      text = 0;
#else
      if (flag == O_RDWR) {
        int fd2;
        if ((fd2 = open__(fileName.c_str(), O_RDWR)) >= 0) {
          write(fd2, text, length);
          close__(fd2);
        }
      }
      delete [] text;
#endif
    }

    text = 0;
  }

  Mmap(): text(0), fd(-1), isArchiveSection(false) {}
#endif

  virtual ~Mmap() { this->close(); }
};
}
#endif
