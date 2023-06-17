/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

#ifndef ZTIMER
#define ZTIMER

#include "common.h"

namespace FastPForLib {

//
// VS2012 bug: high_precision_clock is defined as system_clock and precision is
// about 15 MS!!
// See: https://connect.microsoft.com/VisualStudio/feedback/details/719443
//
// Implementation has been taken from a post on stackoverflow and adapted here
// http://stackoverflow.com/questions/13263277/difference-between-stdsystem-clock-and-stdsteady-clock
//
#ifdef _WIN32
#define NOMINMAX
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

struct qpc_clock {
  typedef std::chrono::nanoseconds duration;
  typedef duration::rep rep;
  typedef duration::period period;
  typedef std::chrono::time_point<qpc_clock, duration> time_point;
  static time_point now() {
    static bool isInited = false;
    static LARGE_INTEGER frequency = {0, 0};
    if (!isInited) {
      if (QueryPerformanceFrequency(&frequency) == 0) {
        throw std::logic_error("QueryPerformanceCounter not supported: " +
                               std::to_string(GetLastError()));
      }
      isInited = true;
    }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return time_point(duration(static_cast<rep>((double)counter.QuadPart /
                                                frequency.QuadPart *
                                                period::den / period::num)));
  }
};

#endif

/**
 *  author: Preston Bannister
 */
class WallClockTimer {
public:
#ifdef _WIN32
  typedef qpc_clock clock;
#else
  typedef std::chrono::high_resolution_clock clock;
#endif

  std::chrono::time_point<clock> t1, t2;
  WallClockTimer() : t1(), t2() {
    t1 = clock::now();
    t2 = t1;
  }
  void reset() {
    t1 = clock::now();
    t2 = t1;
  }
  uint64_t elapsed() {
    std::chrono::microseconds delta =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    return delta.count();
  }
  uint64_t split() {
    t2 = clock::now();
    return elapsed();
  }
};

#ifndef _WIN32

class CPUTimer {
public:
  // clock_t t1, t2;
  struct rusage t1, t2;

  CPUTimer() : t1(), t2() {
    getrusage(RUSAGE_SELF, &t1);
    // t1 = clock();
    t2 = t1;
  }
  void reset() {
    getrusage(RUSAGE_SELF, &t1);
    t2 = t1;
  }
  // proxy for userelapsed
  uint64_t elapsed() { return totalelapsed(); }

  uint64_t totalelapsed() { return userelapsed() + systemelapsed(); }
  // returns the *user* CPU time in micro seconds (mu s)
  uint64_t userelapsed() {
    return ((t2.ru_utime.tv_sec - t1.ru_utime.tv_sec) * 1000ULL * 1000ULL) +
           ((t2.ru_utime.tv_usec - t1.ru_utime.tv_usec));
  }

  // returns the *system* CPU time in micro seconds (mu s)
  uint64_t systemelapsed() {
    return ((t2.ru_stime.tv_sec - t1.ru_stime.tv_sec) * 1000ULL * 1000ULL) +
           ((t2.ru_stime.tv_usec - t1.ru_stime.tv_usec));
  }

  uint64_t split() {
    getrusage(RUSAGE_SELF, &t2);
    return elapsed();
  }
};

#endif

} // namespace FastPFor

#endif
