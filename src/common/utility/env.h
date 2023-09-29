#pragma once

#include <dirent.h>
#include <fcntl.h>
#include <cerrno>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <sys/sysctl.h>
#else

#include <linux/falloc.h>
#include <linux/fiemap.h>
#include <linux/ioctl.h>
#include <linux/magic.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <sys/vfs.h>

#endif  // defined(__APPLE__)

#ifdef __APPLE__
#define fdatasync fsync
#define fread_unlocked fread
#endif

#if defined(__APPLE__)
// Simulates Linux's fallocate file preallocation API on OS X.
int fallocate(int fd, int mode, off_t offset, off_t len);
#endif