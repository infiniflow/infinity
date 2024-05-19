//  MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//  $Id: thread.h 173 2009-04-18 08:10:57Z taku-ku $;
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_THREAD_H
#define MECAB_THREAD_H

#include "common.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif
#endif

#if defined HAVE_PTHREAD_H
#define MECAB_USE_THREAD 1
#endif

#if (defined(_WIN32) && !defined(__CYGWIN__))
#define MECAB_USE_THREAD 1
#endif

#define BEGINTHREAD(src, stack, func, arg, flag, id)                    \
  (HANDLE)_beginthreadex((void *)(src), (unsigned)(stack),              \
                         (unsigned(_stdcall *)(void *))(func), (void *)(arg), \
                         (unsigned)(flag), (unsigned *)(id))

namespace MeCab {

class thread {
 private:
#ifdef HAVE_PTHREAD_H
  pthread_t hnd;
#else
#ifdef _WIN32
  HANDLE  hnd;
#endif
#endif

 public:
  static void* wrapper(void *ptr) {
    thread *p = static_cast<thread *>(ptr);
    p->run();
    return 0;
  }

  virtual void run() {}

  void start() {
#ifdef HAVE_PTHREAD_H
    int res = pthread_create(&hnd, 0, &thread::wrapper,
                   static_cast<void *>(this));
    CHECK_DIE(res == 0) << "pthread_create failed: " << res;
#else
#ifdef _WIN32
    DWORD id;
    hnd = BEGINTHREAD(0, 0, &thread::wrapper, this, 0, &id);
#endif
#endif
  }

  void join() {
#ifdef HAVE_PTHREAD_H
    pthread_join(hnd, 0);
#else
#ifdef _WIN32
    WaitForSingleObject(hnd, INFINITE);
    CloseHandle(hnd);
#endif
#endif
  }

  virtual ~thread() {}
};
}
#endif
