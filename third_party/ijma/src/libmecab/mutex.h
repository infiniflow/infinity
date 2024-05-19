//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: mutex.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_MUTEX_H
#define MECAB_MUTEX_H

#include <string>
#include "common.h"
#include "string_buffer.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef MECAB_WITHOUT_MUTEX_LOCK
#ifdef HAVE_PTHREAD_H
#define _USE_PTHREAD_MUTEX
#include <pthread.h>
#else
#if defined(_WIN32) && !defined(__CYGWIN__)
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#define _USE_WINDOWS_MUTEX
#endif
#endif
#endif

namespace MeCab {

class Mutex {
 private:
#ifdef _USE_PTHREAD_MUTEX
  pthread_mutex_t mutex_;
#else
#ifdef _USE_WINDOWS_MUTEX
  CRITICAL_SECTION mutex_;
#endif
#endif
  whatlog what_;

 public:
  const char *what() { return what_.str(); }

#ifdef _USE_PTHREAD_MUTEX
  Mutex()       { pthread_mutex_init(&mutex_, NULL); }
  virtual ~Mutex() {  pthread_mutex_destroy(&mutex_); }
  void lock()   { pthread_mutex_lock(&mutex_); }
  void unlock() { pthread_mutex_unlock(&mutex_); }
#else
#ifdef _USE_WINDOWS_MUTEX
  Mutex()       { InitializeCriticalSection(&mutex_); }
  virtual ~Mutex() {
    unlock();
    DeleteCriticalSection(&mutex_);
  }
  void lock()   { EnterCriticalSection(&mutex_); }
  void unlock() { LeaveCriticalSection(&mutex_); }
#else
  Mutex() { WHAT("this machine does not support mutex"); }
  virtual ~Mutex() {}
  void lock()   {}
  void unlock() {}
#endif
#endif
};
}

#endif
